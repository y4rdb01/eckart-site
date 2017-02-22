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
Object subclass: #DivisionExample
	instanceVariableNames: 'numerator denominator'
	classVariableNames: ''
	poolDictionaries: ''
!
DivisionExample class comment: '
	This provides students with a very basic example of a class definition.
'
!

!
DivisionExample methodsFor: 'creating new instances'
!

new
	^ super new.

!

init
	numerator := (Dialog request: 'Enter numerator> ') asNumber.
	denominator := (Dialog request: 'Enter denominator> ') asNumber.
! !

!
DivisionExample methodsFor: 'accessing class instance variables'
!

numerator
	^ numerator

!

denominator
	^ denominator
! !

!
DivisionExample methodsFor: 'performing a sample computation'
!

compute: messageString
	| result |
       	[ result := numerator / denominator ]
               	on: ZeroDivide          "A subclass of Error"
               	do: [: exception |
                       	denominator := 0.00000000001.
                       	exception retry
               	]
               	ensure: [ Dialog warn: 'Denominator is 0.' ].
       	Transcript show: messageString; showCR: result.
! !
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
Object subclass: #IOExample
	instanceVariableNames: 'input output'
	classVariableNames: ''
	poolDictionaries: ''
!
IOExample class comment: '
	This provides students with a very basic I/O example.
'
!

!
IOExample methodsFor: 'initialize'
!

init
	[ input := FileSelectionBrowser open asFilename readStream. ]
		on: Error
		do: [ :exception |
			Dialog error: 'Unable to open file'.
			exception retry.
		].
	[ output := FileSelectionBrowser open asFilename writeStream. ]
		on: Error
		do: [ :exception |
			Dialog error: 'Unable to open file'.
			exception retry.
		].
! !

!
IOExample methodsFor: 'copy input to output turning :: into :'
!

copy
	| data lookAhead|
	[ input atEnd ] whileFalse: [
		data := input nextByte asCharacter.
		(data = $!!)
			ifTrue: [
				(input atEnd) ifFalse: [
					lookAhead := input peek.
					(lookAhead = $!!)
						ifTrue: [ input nextByte asCharacter. ].
					(lookAhead asCharacter isDigit)
						ifTrue: [
							data := input nextDecimalInteger.
				  			input nextByte.
						].
				].
			].
		output show: data.
	].
	input close.
	output close.
! !
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
