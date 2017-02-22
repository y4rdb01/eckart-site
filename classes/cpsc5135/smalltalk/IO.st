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
