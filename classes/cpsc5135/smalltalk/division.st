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
