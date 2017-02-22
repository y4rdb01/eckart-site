Object subclass: #FactorialExample
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
!
FactorialExample class comment: '
	This provides students with a very basic example of a class definition.
'
!

!
FactorialExample methodsFor: 'perform a computation'
!

compute: x
	^ (x <= 1)
		ifTrue: [1]
		ifFalse: [x * (self compute: (x - 1))].
! !
