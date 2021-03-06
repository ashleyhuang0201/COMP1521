# board1.s ... Game of Life on a 10x10 grid

	.data

N:	.word 10  # gives board dimensions

board:
	.byte 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.byte 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
	.byte 0, 0, 0, 1, 0, 0, 0, 0, 0, 0
	.byte 0, 0, 1, 0, 1, 0, 0, 0, 0, 0
	.byte 0, 0, 0, 0, 1, 0, 0, 0, 0, 0
	.byte 0, 0, 0, 0, 1, 1, 1, 0, 0, 0
	.byte 0, 0, 0, 1, 0, 0, 1, 0, 0, 0
	.byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
	.byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
	.byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0

newBoard: .space 100
# COMP1521 19t2 ... Game of Life on a NxN grid
#
# Written by <<z5259842 Yingqi Huang>>, June 2019

## Requires (from `boardX.s'):
# - N (word): board dimensions
# - board (byte[][]): initial board state
# - newBoard (byte[][]): next board state

    .data

ask: 
	.asciiz "# Iterations: "

afteriteration: 
	.asciiz "=== After iteration "
    
equal: 
	.asciiz " === "

eol: 
	.asciiz "\n"
    
hash: 
	.asciiz "#"

dot: 
	.asciiz "."

## Provides:
	.globl	main
	.globl	decideCell
	.globl	neighbours
	.globl	copyBackAndShow


########################################################################
# .TEXT <main>
	.text
main:

	sw    $fp, -4($sp)	# push $fp onto stack
	la    $fp, -4($sp)	# set $fp for this function
	sw    $ra, -4($fp)	# save return address
	sw    $s0, -8($fp)	# $s0 is maxiters
	sw    $s1, -12($fp)	# $s1 is int n;
	sw    $s2, -16($fp)	# $s2 is int i;
	sw    $s3, -20($fp)	# $s3 is int j;
	sw    $s4, -24($fp)	# $s4 is int nn;
	sw    $s5, -28($fp)	# intsize = sizeof(int)
	sw    $s6, -32($fp)	# $s6 is N = 10
	sw    $s7, -36($fp)	# newboard
	addi  $sp, $sp, -40	# reset $sp to last pushed item
    
	la $a0, ask
	li $v0, 4
	syscall			# printf("# Iterations: ")

	li $v0, 5		# scanf int maxiters into $v0
	syscall 
	move $s0, $v0		# int maxiters = $s0

	li $s1, 1   		# int n = 1

for_loop1:
    
	bgt $s1, $s0, end_loop1	# if n > maxiters, branch to end_loop1  
	li $s2, 0   		# int i = 0
	lw $s6, N   		# $s6 is N = 10
    
for_loop2:
    
	bge $s2, $s6, end_loop2	# if i >= N, branch to end_loop2
	li $s3, 0   		# int j = 0

for_loop3:
	bge $s3, $s6, end_loop3	# if j >= N, branch to end_loop3

	move $a0, $s2		# move value of i into $a0
	move $a1, $s3		# move j value into $a1

	jal neighbours  	# call function neighbours
	nop
	move $t3, $v0 		# moves value of nn into $t3

	mul $t0, $s2, $s6 	# store i * N  into $t0
	add $t0, $s3, $t0  	# add j + i * N
    
    
	la $t2, board 		# load board into $t2
	addu $t2, $t0, $t2
	lb $a0, ($t2) 

	move $a1, $t3 		# returns neighbours nn into $a1

	jal decideCell		# call function decideCell
	nop
    
	mul $t0, $s2, $s6 	# store i * N  into $t0
	add $t0, $s3, $t0  	# add j + i * N to calculate the offset

	la $s7, newBoard 	# load newBoard into $s7
	addu $t4, $s7, $t0 	# add offset $t0 and newboard, store in $t4
	sb $v0, ($t4) 		# store byte into $v0

	addi $s3, $s3, 1 	# j++ 
	j for_loop3 		# jump back for_loop3
    
end_loop3:

	addi $s2, $s2, 1 	# i++
	j for_loop2 		# jump to for_loop2
    
end_loop2:

	la $a0, afteriteration
	li $v0, 4
	syscall 		# printf("=== After iteration")

	move $a0, $s1
	li $v0, 1
	syscall 		# printf("%d",n)
    
	la $a0, equal
	li $v0, 4
	syscall 		# printf("===")
    
	la $a0, eol
	li $v0, 4 		# printf("\n") char
	syscall

	jal copyBackAndShow 	# calls the function copyBackAndShow()
	nop 			# does nothing

	addi $s1, $s1, 1 	# n++

	j for_loop1 		# jump to for_loop1
    
end_loop1:

    				# clean up stack frame
	lw    $s7, -36($fp)	# restore $s7 value   
	lw    $s6, -32($fp)     # restore $s6 value
	lw    $s5, -28($fp)     # restore $s5 value
	lw    $s4, -24($fp)     # restore $s4 value
	lw    $s3, -20($fp)     # restore $s3 value
	lw    $s2, -16($fp)     # restore $s2 value
	lw    $s1, -12($fp)     # restore $s1 value
	lw    $s0, -8($fp)      # restore $s0 value
	lw    $ra, -4($fp)      # restore $ra for return
	la    $sp, 4($fp)       # restore $sp (remove stack frame)
	lw    $fp, ($fp)        # restore $fp (remove stack frame) 

	li    $v0, 0
	jr    $ra                
	
.text

decideCell:
	sw    $fp, -4($sp)	# push $fp onto stack
        la    $fp, -4($sp)      # set $fp for this function
        sw    $ra, -4($fp)      # save return address
        sw    $s0, -8($fp)      # $s0 is int old
        sw    $s1, -12($fp)     # $s1 is int nn;
        addi  $sp, $sp, -16     # reset $sp to last pushed item
        

        move $s0, $a0 		# move value of old in $a0 into $s0
        move $s1, $a1 		# move value of nn in $a1 into $s1

        li $t0, 1 		# constant 1
        li $t1, 2 		# constant 2
        li $t2, 3 		# constant 3

        bne $s0, $t0, else_if_2 # if old != 1, branch to else_if_2
        
        bge $s1, $t1, else_if_1 # if nn >= 2, branch to else_if_1
        
        li $v0, 0 		# ret = 0 
        j very_end
        
else_if_1:
        beq $s1, $t1, truee_1  	# if nn == 2, branch to truee_1
        beq $s1, $t2, truee_1 	# if nn == 3, branch to truee_1
        
        j else_1
        
truee_1:
        li $v0, 1 		# ret = 1
        j very_end
        
else_1:
        li $v0, 0 		# ret = 0
        j very_end
        
else_if_2:
        bne $s1, $t2, else_2 	# nn != 3
        li $v0, 1 		# ret = 1
        j very_end
        
else_2: 
        li $v0, 0 		# ret = 0 
        j very_end
        
very_end:
        
      
        
	lw    $s1, -12($fp)	# restore $s1 value
	lw    $s0, -8($fp)      # restore $s0 value
	lw    $ra, -4($fp)      # restore $ra for return
	la    $sp, 4($fp)       # restore $sp (remove stack frame)
	lw    $fp, ($fp)        # restore $fp (remove stack frame)

	jr    $ra                
        
.text

neighbours:

	sw    $fp, -4($sp)	# push $fp onto stack
	la    $fp, -4($sp)      # set $fp for this function
	sw    $ra, -4($fp)      # save return address
	sw    $s0, -8($fp)      # $s0 is constant 1
	sw    $s1, -12($fp)     # $s1 is int n
	sw    $s2, -16($fp)     # $s2 is lb 
	addi  $sp, $sp, -20     # reset $sp to last pushed item

	li  $t0, 0 		# int nn = 0
	li  $t1, -1 		# $t1 is int x = -1
	li  $s0, 1 		# constant 1
    
start_for_1:
    
	bgt $t1, $s0, end_for_1 # if x > 1

	li $t2, -1 		# $t2 is int y = -1
    
start_for_2:
	bgt $t2, $s0, end_for_2 # if y > 0, branch to end_for_2
	add $t3, $t1, $a0 	# $t3 = i + x

	bltz $t3, incr_loop_2 	# if i + x < 0, continue

	lw $t4, N 		# load N = 10 into $t4
	addi $t4, $t4, -1 	# $t4 = N - 1
    
    
    	bgt $t3, $t4, incr_loop_2 # if i + x > N - 1, continue
    
    	add $t5, $t2, $a1 	# $t5 = j + y
    	bltz $t5, incr_loop_2 	# if j + y < 0, continue
    
    	bgt $t5, $t4, incr_loop_2 # if j + y > N - 1, continue
    
    	bnez $t1, skip_if 	# if $t1 != 0, branch to skip_if
    	bnez $t2, skip_if 	# if $t2 != 0, branch to skip_if
    
    	j incr_loop_2 		# x == 0 && y == 0, jump to incr_loop_2
    
skip_if: 			# if board[i + x][j + y] == 1, nn+ 
    
    	la $t6, board 		# address of board

    	add $t7 , $a0, $t1 	# add i + x

    	lw $t4, N 		# $t4 is N = 10
    	mul $t7, $t7, $t4 	# (i + x) * N
	add $t7, $t7, $a1 	# (i + x) * N + j
    	add $t7, $t7, $t2 	# add $t2 to (i + x) * N + j
    
    	addu $t7, $t7, $t6
    
	lb $s1, ($t7)
	li $s2, 1
	bne $s1, $s2, incr_loop_2 # if board[i + x][j + y != 1, branch to incr_loop_2
	addi $t0, $t0, 1 	# nn++
    
incr_loop_2: 			# increment loop 2
	addi $t2, $t2, 1 	# y++
	j start_for_2
    
end_for_2: 			# increment loop 1
    
    	addi $t1, $t1, 1 	# x++
	j start_for_1
    
end_for_1:
	move $v0, $t0 		# move nn into $v0
    
	lw    $s2, -16($fp)     # restore $s2 value
	lw    $s1, -12($fp)     # restore $s1 value
	lw    $s0, -8($fp)      # restore $s0 value
	lw    $ra, -4($fp)      # restore $ra for return
	la    $sp, 4($fp)       # restore $sp (remove stack frame)
	lw    $fp, ($fp)        # restore $fp (remove stack frame) 

	jr    $ra               
    
    
.text

copyBackAndShow:

	sw    $fp, -4($sp)  	# push $fp onto stack
	la    $fp, -4($sp)      # set $fp for this function
	sw    $ra, -4($fp)      # save return address
	sw    $s0, -8($fp)      # $s0 is int i
	sw    $s1, -12($fp)     # $s1 is int j
	addi  $sp, $sp, -16     # reset $sp to last pushed item

	lw $t0, N 		# N = 10 into $t0
	li $s0, 0 		# int i = 0
    
loop1:
    
	bge $s0, $t0, end_1 	# if i >= N, branch to end_1
    
	li $s1, 0 		# int j = 0
    
loop2:
	bge $s1, $t0, end_2 	# if j >= N, branch to end_2

	la $t1, board 		# load board
	la $t2, newBoard 	# load newBoard

	mul $t3, $s0, $t0 	# $t3 = N * i
	add $t3, $s1, $t3 	# $t3 = i *N + j

	add $t4, $t2, $t3 	# newBoard[i][j]
	add $t5, $t1, $t3 	# board[i][j]

	lb $t6, ($t4)
	sb $t6, ($t5)
    
	bnez $t6, else 		# if board[i][j] != 0, branch to else

	la $a0, dot
	li $v0, 4
	syscall 		# printf(".")
	j if_end
   
    
else:
	la $a0, hash
	li $v0, 4
	syscall 		# printf("#")
    
if_end:
	addi $s1, $s1, 1 	# j++
	j loop2
    
end_2:
	la $a0, eol
	li $v0, 4
	syscall 		#printf("\n")
    
	addi $s0, $s0, 1 	#i++
	j loop1
    
end_1:
    

	lw    $s1, -12($fp)   	# restore $s1 value
	lw    $s0, -8($fp)      # restore $s0 value
	lw    $ra, -4($fp)      # restore $ra for return
	la    $sp, 4($fp)       # restore $sp (remove stack frame)
	lw    $fp, ($fp)        # restore $fp (remove stack frame)

	jr    $ra              

    




