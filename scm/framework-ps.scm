
(define-module (scm framework-ps))

(use-modules (ice-9 regex)
	     (ice-9 string-fun)
	     (ice-9 format)
	     (guile)
	     (srfi srfi-13)
	     (scm output-ps)
	     (lily))

(define (tex-font? fontname)
  (equal? (substring fontname 0 2) "cm"))


(define (load-fonts bookpaper)
  
  (let*
      ((fonts (ly:bookpaper-fonts bookpaper))
       (font-names (uniq-list (sort (map ly:font-filename fonts) string<?)))
       (pfas (map
	      (lambda (x)
		(ly:kpathsea-gulp-file (string-append x ".pfa")))
	      
	      (filter string? font-names)))
       )

    (string-join pfas "\n")))


(define (define-fonts bookpaper)

  (define font-list (ly:bookpaper-fonts bookpaper))
  (define (define-font command fontname scaling)
    (string-append
     "/" command " { /" fontname " findfont "
     (ly:number->string scaling) " output-scale div scalefont } bind def\n"))

  (define (reencode-font plain encoding command)
    (let ((coding-vector (get-coding-command encoding)))
      (string-append
       plain " " coding-vector " /" command " reencode-font\n"
       "/" command "{ /" command " findfont 1 scalefont } bind def\n")))
  
  (define (guess-ps-fontname basename)
    
    "We do not have the FontName, try to guess is from basename."
    (cond
     (#t basename)
     ((tex-font? basename)
      ;; FIXME: we need proper Fontmap for the bluesky CM*, EC* fonts.
      ;; Only the fonts that we trace in mf/ are in our own FontMap.
      (string-append basename ".pfb"))
     (else (string-append basename ".pfa"))
     ))

  (define (font-load-command font)
    (let* ((specced-font-name (ly:font-name font))
	   (fontname (if specced-font-name
			 specced-font-name
			 (guess-ps-fontname (ly:font-filename font))))
	   
	   (coding-alist (ly:font-encoding-alist font))
	   (input-encoding (assoc-get 'input-name coding-alist))
	   (font-encoding (assoc-get 'output-name coding-alist))
	   (command (ps-font-command font))
	   ;; FIXME -- see (ps-font-command )
	   (plain (ps-font-command font #f))
	   (designsize (ly:font-design-size font))
	   (magnification (* (ly:font-magnification font)))
	   (ops (ly:output-def-lookup bookpaper 'outputscale))
	   (scaling (* ops magnification designsize)))

      (string-append
       (define-font plain fontname scaling)
       (if (or (equal? input-encoding font-encoding)
	       ;; guh
	       (equal? font-encoding "fetaBraces")
	       (equal? font-encoding "fetaNumber")
	       (equal? font-encoding "fetaMusic")
	       (equal? font-encoding "parmesanMusic"))
	       ""
	   (reencode-font plain input-encoding command)))))
  
  (define (font-load-encoding encoding)
    (let ((filename (get-coding-filename encoding)))
      (ly:kpathsea-gulp-file filename)))

  (let* ((encoding-list (map (lambda (x)
			       (assoc-get 'input-name
					  (ly:font-encoding-alist x)))
			     font-list))
	 (encodings (uniq-list (sort-list (filter string? encoding-list)
					  string<?))))

    (string-append
     (apply string-append (map font-load-encoding encodings))
     (apply string-append
	    (map (lambda (x) (font-load-command x)) font-list)))))

;; FIXME: duplicated in other output backends
;; FIXME: silly interface name
(define (output-variables paper)
  ;; FIXME: duplicates output-paper's scope-entry->string, mostly
  (define (value->string  val)
    (cond
     ((string? val) (string-append "(" val ")"))
     ((symbol? val) (symbol->string val))
     ((number? val) (number->string val))
     (else "")))
  
  (define (output-entry ps-key ly-key)
    (string-append
     "/" ps-key " " (value->string (ly:output-def-lookup paper ly-key)) " def \n"))
  
  (string-append
   "/lily-output-units 2.83464  def  %% milimeter \n"
   "% /lily-output-units 0.996264  def  %% true points.\n"
   (output-entry "staff-line-thickness" 'linethickness)
   (output-entry "line-width" 'linewidth)
   (output-entry "paper-size" 'papersize)
   (output-entry "staff-height" 'staffheight)	;junkme.
   "/output-scale "
   (number->string (ly:output-def-lookup paper 'outputscale))
   " lily-output-units mul def \n"
    ))
  
(define (header paper page-count classic?)
  (string-append
   "%!PS-Adobe-3.0\n"
   "%%Creator: creator time-stamp \n"
   ))

(define (dump-page outputter page page-number page-count)
  (ly:outputter-dump-string outputter
   (string-append
    "%%Page: " (number->string page-number) " " (number->string page-count) "\n"
    "0 0 start-system { "
    "set-ps-scale-to-lily-scale "
    "\n"))
  (ly:outputter-dump-stencil outputter (ly:page-stencil page))
  (ly:outputter-dump-string outputter
			    "} stop-system \nshowpage\n") )
  
  
(define-public (output-framework-ps outputter book scopes fields basename)
  (let*
      ((bookpaper  (ly:paper-book-book-paper book))
       (pages (ly:paper-book-pages book))
       (pageno 0)
       (page-count (length pages))
       )
  (for-each
   (lambda (x)
     (ly:outputter-dump-string outputter x))
   (list
    (header bookpaper
	    (length pages)
	    #f)

   "%%Pages: " (number->string page-count) "\n"
   "%%PageOrder: Ascend\n"
   "%%DocumentPaperSizes: " (ly:output-def-lookup bookpaper 'papersize) "\n"
    
    (output-variables bookpaper)
    (ly:gulp-file "music-drawing-routines.ps")
    (ly:gulp-file "lilyponddefs.ps")
    (define-fonts bookpaper)
    ))

  (for-each
   (lambda (page)
     (set! pageno (1+ pageno))
     (dump-page outputter page pageno page-count))
   pages)
  (ly:outputter-dump-string outputter "%%Trailer\n%%EOF\n")
  ))



  
(define-public (output-classic-framework-ps outputter book scopes fields basename)
  (let*
      ((bookpaper  (ly:paper-book-book-paper book))
       (lines (ly:paper-book-lines book))
       (y 0.0)
       (scale (* 2.83464 (ly:output-def-lookup bookpaper 'outputscale)))
       (total-y (apply + (map (lambda (z) (ly:paper-line-extent z Y))  lines)))
       (x-ext '(-8 . 0))
       (lineno 0)
       )
    
    (define (dump-line outputter system)
      (let*
	  ((stil  (ly:paper-line-stencil  system)))
	   
      (ly:outputter-dump-string
       outputter
       (string-append
	" 0.0 "
	(ly:number->string y)
	" start-system {\n set-ps-scale-to-lily-scale\n"))
      (set! y (+ y (ly:paper-line-extent system Y)))
      (ly:outputter-dump-stencil outputter stil)
      (ly:outputter-dump-string
       outputter
       "} stop-system\n")))

    (define (to-pt x)
      (inexact->exact (round (* scale x))))
    (for-each (lambda (l)
		(set! x-ext (interval-union x-ext (cons 0.0 (ly:paper-line-extent l X))))
		)
		lines)
  (for-each
   (lambda (x)
     (ly:outputter-dump-string outputter x))
   (list
    "%!PS-Adobe-2.0 EPSF-2.0\n"
    "%%Creator: LilyPond \n"
    "%%BoundingBox: "
    (ly:number->string (to-pt (car x-ext))) " "
    (ly:number->string (to-pt 0)) " " 
    (ly:number->string (to-pt (cdr x-ext))) " "
    (ly:number->string (to-pt total-y)) "\n"
    "%%EndComments\n"
    (output-variables bookpaper)
    (ly:gulp-file "music-drawing-routines.ps")
    (ly:gulp-file "lilyponddefs.ps")
    (load-fonts bookpaper)
    (define-fonts bookpaper)
    ))

  (for-each
   (lambda (line)
     (set! lineno (1+ lineno))
     (dump-line outputter line)) ;   pageno page-count))
   lines)
  (ly:outputter-dump-string outputter "\n")
  ))

