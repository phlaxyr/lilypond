/*
  source-file.cc -- implement Source_file

  source file of the GNU LilyPond music typesetter

  (c) 1997--2006 Jan Nieuwenhuizen <janneke@gnu.org>
  Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "source-file.hh"

#include "config.hh"

#if HAVE_UTF8_WCHAR_H
#include <utf8/wchar.h>  /* mbrtowc */
#else
#include <cwchar> /* mbrtowc */
#endif

#include <cstdio>

#if HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#define istringstream(x) istrstream (x, length ())
#endif
using namespace std;

#include "file-name-map.hh"
#include "international.hh"
#include "warn.hh"

void
Source_file::load_stdin ()
{
  length_ = 0;
  chs_.clear ();
  int c;
  while ((c = fgetc (stdin)) != EOF)
    chs_.push_back (c);

  chs_.push_back (0);
  length_ = chs_.size ();
  contents_str0_ = &chs_[0];
}

char *
gulp_file (std::string filename, int *filesize)
{
  /* "b" must ensure to open literally, avoiding text (CR/LF)
     conversions.  */
  FILE *f = fopen (filename.c_str (), "rb");
  if (!f)
    {
      warning (_f ("can't open file: `%s'", filename.c_str ()));
      return 0;
    }

  fseek (f, 0, SEEK_END);
  int real_size = ftell (f);
  int read_count = real_size;

  if (*filesize >= 0)
    read_count = min (read_count, *filesize);
  
  rewind (f);

  char *str = new char[read_count + 1];
  str[read_count] = 0;

  int bytes_read = fread (str, sizeof (char), read_count, f);
  if (bytes_read != read_count)
    warning (_f ("expected to read %d characters, got %d", bytes_read,
		 read_count));
  fclose (f);
  *filesize = bytes_read;
  return str;
}

Source_file::Source_file (std::string filename, std::string data)
{
  name_ = filename;
  istream_ = 0;
  length_ = data.length ();
  contents_str0_ = string_copy (data);
  pos_str0_ = c_str ();
  init_port ();

  for (int i = 0; i < length_; i++)
    if (contents_str0_[i] == '\n')
      newline_locations_.push_back (contents_str0_ + i);
}

Source_file::Source_file (std::string filename_string)
{
  name_ = filename_string;
  istream_ = 0;
  contents_str0_ = 0;

  if (filename_string == "-")
    load_stdin ();
  else
    {
      length_ = -1;
      contents_str0_ = gulp_file (filename_string, &length_);
    }
  
  pos_str0_ = c_str ();

  init_port ();

  for (int i = 0; i < length_; i++)
    if (contents_str0_[i] == '\n')
      newline_locations_.push_back (contents_str0_ + i);
}

void
Source_file::init_port ()
{
  SCM str = scm_makfrom0str (contents_str0_);
  str_port_ = scm_mkstrport (SCM_INUM0, str, SCM_OPN | SCM_RDNG, __FUNCTION__);
  scm_set_port_filename_x (str_port_, scm_makfrom0str (name_.c_str ()));
}

int
Source_file::tell () const
{
  return pos_str0_ - contents_str0_;
}

istream *
Source_file::get_istream ()
{
  if (!istream_)
    {
      if (length ()) // can-t this be done without such a hack?
	istream_ = new istringstream (c_str ());
      else
	{
	  istream_ = new istringstream ("");
	  istream_->setstate (ios::eofbit);
	  //	  istream_->set (ios::eofbit);
	}
    }
  return istream_;
}

std::string
Source_file::file_line_column_string (char const *context_str0) const
{
  if (!c_str ())
    return " (" + _ ("position unknown") + ")";
  else
    {
      int l, ch, col;
      get_counts (context_str0, &l, &ch, &col);

      return name_string () + ":" + to_string (l)
	+ ":" + to_string (col);
    }
}

std::string
Source_file::quote_input (char const *pos_str0) const
{
  if (!contains (pos_str0))
    return " (" + _ ("position unknown") + ")";

  int l, ch, col;
  get_counts (pos_str0, &l, &ch, &col);
  std::string line = line_string (pos_str0);
  std::string context = line.substr (0, ch)
    + to_string ('\n')
    + to_string (' ', col)
    + line.substr (ch, line.length()-ch);
  return context;
}

std::string
Source_file::name_string () const
{
  return map_file_name (name_);
}

Source_file::~Source_file ()
{
  delete istream_;
  istream_ = 0;
  delete[] contents_str0_;
}

Slice
Source_file::line_slice (char const *pos_str0) const
{
  if (!contains (pos_str0))
    return Slice (0, 0);

  char const *data_str0 = c_str ();
  char const *eof_C_ = data_str0 + length ();

  if (pos_str0 == eof_C_)
    pos_str0--;
  char const *begin_str0 = pos_str0;
  while (begin_str0 > data_str0)
    if (*--begin_str0 == '\n')
      {
	begin_str0++;
	break;
      }

  char const *end_str0 = pos_str0;
  while (end_str0 < eof_C_)
    if (*end_str0++ == '\n')
      {
	end_str0--;
	break;
      }

  return Slice (begin_str0 - data_str0, end_str0 - data_str0);
}

std::string
Source_file::line_string (char const *pos_str0) const
{
  if (!contains (pos_str0))
    return "";

  Slice line = line_slice (pos_str0);
  char const *data_str0 = c_str ();
  return std::string (data_str0 + line[LEFT], line.length ());
}

void
Source_file::get_counts (char const *pos_str0,
			 int *line_number,
			 int *line_char,
			 int *column) const
{
  *line_number = 0;
  *line_char = 0;
  *column = 0;
    
  if (!contains (pos_str0))
    return;

  *line_number = get_line (pos_str0);

  Slice line = line_slice (pos_str0);
  char const *data = c_str ();
  char const *line_start = (char const *)data + line[LEFT];

  ssize left = (char const *) pos_str0 - line_start;
  std::string line_begin (line_start, left);
  char const *line_chars = line_begin.c_str ();

  *column = 0;
  *line_char = 0;

  mbstate_t state;

  /* Initialize the state.  */
  memset (&state, '\0', sizeof (state));

  while (left > 0)
    {
      wchar_t multibyte[2];

      /*
	FIXME, this is apparently locale dependent.
      */
#if HAVE_MBRTOWC
      size_t thislen = mbrtowc (multibyte, line_chars, left, &state);
#else
      size_t thislen = 1;
#endif /* !HAVE_MBRTOWC */

      /* Stop converting at invalid character;
	 this can mean we have read just the first part
	 of a valid character.  */
      if (thislen == (size_t) -1)
	break;

      /* We want to handle embedded NUL bytes
	 but the return value is 0.  Correct this.  */
      if (thislen == 0)
	thislen = 1;

      if (thislen == 1 && line_chars[0] == '\t')
	(*column) = (*column / 8 + 1) * 8;
      else
	(*column)++;

      (*line_char)++;
      /* Advance past this character. */
      line_chars += thislen;
      left -= thislen;
    }
}

bool
Source_file::contains (char const *pos_str0) const
{
  return (pos_str0 && (pos_str0 >= c_str ()) && (pos_str0 <= c_str () + length ()));
}

int
Source_file::get_line (char const *pos_str0) const
{
  if (!contains (pos_str0))
    return 0;

  if (!newline_locations_.size ())
    return 1;

  int lo = 0;
  int hi = newline_locations_.size ();

  if (newline_locations_[lo] > pos_str0)
    return 1;

  if (newline_locations_[hi - 1] < pos_str0)
    return hi;

  binary_search_bounds (newline_locations_,
			pos_str0,
			default_compare,
			&lo, &hi);

  if (*pos_str0 == '\n')
    lo--;
  return lo + 2;
}

int
Source_file::length () const
{
  return length_;
}

char const *
Source_file::c_str () const
{
  return contents_str0_;
}

void
Source_file::set_pos (char const *pos_str0)
{
  if (contains (pos_str0))
    pos_str0_ = pos_str0;
  else
    error (quote_input (pos_str0) + "invalid pos");
}

char const *
Source_file::seek_str0 (int n)
{
  char const *new_str0 = c_str () + n;
  if (n < 0)
    new_str0 += length ();
  if (contains (new_str0))
    pos_str0_ = new_str0;
  else
    error (quote_input (new_str0) + "seek past eof");

  return pos_str0_;
}

char const *
Source_file::forward_str0 (int n)
{
  char const *old_pos = pos_str0_;
  char const *new_str0 = pos_str0_ + n;
  if (contains (new_str0))
    pos_str0_ = new_str0;
  else
    error (quote_input (new_str0) + "forward past eof");

  return old_pos;
}

std::string
Source_file::get_string (int n)
{
  std::string str = std::string ((char const *)forward_str0 (n), n);
  return str;
}

SCM
Source_file::get_port () const
{
  return str_port_;
}
