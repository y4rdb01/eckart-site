; Numbers, t, nil [and ()] should evaluate to themselves
1
t
nil
()

; This should produce: a
;
(car (quote (a b)))

; This should produce: a
;
(car (quote (a . b)))

; This should produce: (b)
;
(cdr (quote (a b)))

; This should produce: b
;
(cdr (quote (a . b)))

; This should produce: (a . b)
;
(cons (quote a) (quote b))

; This should produce: (a b c d)
;
(cons (quote a) (cons (quote b) (cons (quote c) (cons (quote d) nil))))

; This should produce: (a b c . d)
;
(cons (quote a) (cons (quote b) (cons (quote c) (quote d))))

; This should produce: a
;
(car (cons (quote a) (quote b)))

; This should produce: b
;
(cdr (cons (quote a) (quote b)))

; This should produce: c
;
(car (cdr (cdr (cons (quote a)
		     (cons (quote b)
			   (cons (quote c)
				 (cons (quote d) nil)
			   )
		     )
	       ))))

; This should produce: nil
;
(cdr (cdr (cdr (cdr (cdr (cdr (cdr (cons (quote a)
					 (cons (quote b)
					       (cons (quote c)
						     (cons (quote d)
							   nil)))))))))))

; Atoms with spaces must appear in "s
(car (quote ("first one" "second one" third)))

; This should produce: b
;
(and (atom (quote a)) (quote b))

; This should produce: nil
;
(and (not (atom (quote a))) (quote b))

; This should produce: t
;
(or (atom (quote a)) (quote b))

; This should produce: b
;
(or (not (atom (quote a))) (quote b))

; This should produce: nil
;
(eq (quote a) (quote b))

; This should produce: t
;
(eq (quote a) (quote a))

; This should produce: nil
;
(eq (quote a) (cons (quote a) (quote b)))

; This should produce: nil
;
(eq (cons (quote a) (quote b)) (cons (quote a) (quote b)))

; This should produce: 19
;
(+ (- 3 5) (* 7 (/ 9 3)))

; This should produce: nil
;
(< nil 4)

; This should produce: t
;
(< 4 12)

; This should produce: nil
;
(> 4 12)

; This should produce: nil
;
(> 4 nil)

; Define a simple fibonacci function.
;
(defun fib (x)
	(or
		(and (< x 2) 1)
		(+ (fib (- x 1)) (fib (- x 2)))
	)
)

; A simple countdown loop.
;
(defun countdown (x)
	(and
		(write "The next number is ")
		(write x)
		(writenl)
		(or
			(and 
				(> x 0)
				(countdown (- x 1))
			)
			t
		)
	)
)

; A map function that works with sublists.
;
(defun maplists (function listOfLists)
	(cond
		((not listOfLists) nil)
		(t (cons (eval (cons function (car listOfLists)))
			 (maplists function (cdr listOfLists)))
		)
	)
)
