;;;; lily.scm -- implement Scheme output routines for TeX and PostScript
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c) 1998--2000 Jan Nieuwenhuizen <janneke@gnu.org>
;;;; Han-Wen Nienhuys <hanwen@cs.uu.nl>

;;; Library funtions

(use-modules (ice-9 regex))

;;(write standalone (current-error-port))

;;; General settings

(debug-enable 'backtrace)

(define point-and-click #f)
(define security-paranoia #f)
(define default-script-alist '())
(define font-name-alist  '())

(if (not (defined? 'standalone))
    (define standalone (not (defined? 'ly-gulp-file))))

;; The regex module may not be available, or may be broken.
(define use-regex
  (let ((os (string-downcase (vector-ref (uname) 0))))
    (not (equal? "cygwin" (substring os 0 (min 6 (string-length os)))))))

;; If you have trouble with regex, define #f
(define use-regex #t)
;;(define use-regex #f)


;;; Un-assorted stuff

;; URG guile-1.3/1.4 compatibility
(define (ly-eval x) (eval2 x #f))

(define (sign x)
  (if (= x 0)
      1
      (if (< x 0) -1 1)))


;;(define major-scale
;;  '(
;;    (0 . 0)
;;    (1 . 0)
;;    (2 . 0)
;;    (3 . 0)
;;    (4 . 0)
;;    (5 . 0)
;;    (6 . 0)
;;   ))


(map (lambda (x) (eval-string (ly-gulp-file x)))
     '("output-lib.scm"
       "tex.scm"
       "ps.scm"
       ;; "ascii-script.scm"
       ))

(if (not standalone)
    (map (lambda (x) (eval-string (ly-gulp-file x)))
	 '("c++.scm"
	   "backend-property.scm"
	   "translator-properties.scm"
	   "interface.scm"
	   "beam.scm"
	   "clef.scm"
	   "slur.scm"
	   "font.scm"
	   "auto-beam.scm"
	   "generic-property.scm"
	   "basic-properties.scm"
	   "chord-name.scm"
	   "element-descriptions.scm"
	   )))
