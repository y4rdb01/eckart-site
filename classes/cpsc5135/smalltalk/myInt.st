Object subclass: #MyIntExample
	instanceVariableNames: 'value'
	classVariableNames: ''
	poolDictionaries: ''
!
MyIntExample class comment: '
	This provides students with a very basic example of a class definition.
'
!

!
MyIntExample methodsFor: 'creating and setting values'
!

setValue: x
	value := x.
! !

!
MyIntExample methodsFor: 'instance variable access'
!

getValue
	^ value.
! !

!MyIntExample methodsFor: 'performing a sample computation and showing values'!

+: x
       	^ MyIntExample new setValue: (value + (x getValue)).

!

printOn: stream
	stream nextPutAll: value.
	stream cr.
! !
