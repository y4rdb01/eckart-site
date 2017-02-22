Object subclass: #BinarySearchExample
	instanceVariableNames: 'list'
	classVariableNames: ''
	poolDictionaries: ''
!
BinarySearchExample class comment: '
	This provides students with a very basic example of a class definition.
'
!

!
BinarySearchExample methodsFor: 'creating new instances'
!

init
	list := Array new: 256.
	1 to: (list capacity) do: [: j |
		list at: j put: (j squared).
	].
	^ self.
! !

!
BinarySearchExample methodsFor: 'find a value if it is there'
!

find: key
	| lowerBound midPoint upperBound |
	lowerBound := 1.
	upperBound := (list size).
	[lowerBound <= upperBound] whileTrue: [
		| test |
		midPoint := lowerBound + upperBound // 2.
		test := list at: midPoint.
		test = key
			ifTrue: [ ^ midPoint ]
			ifFalse: [
				(test < key)
					ifTrue: [ lowerBound := midPoint + 1 ]
					ifFalse: [ upperBound := midPoint - 1 ]
			]
	].
	^ 0.
! !
