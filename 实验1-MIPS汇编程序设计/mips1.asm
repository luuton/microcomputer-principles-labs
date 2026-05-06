.data
arr1:       .word 1, -4, 8, -9, 5, 6, -10, 19, 22, 23
arr2:       .word 121, -124, 138, -199, 255, 2566, -1034, 1019, 2032, 2033

msg_case1:  .asciiz "Case1: SP = "
msg_case2:  .asciiz "Case2: SP = "
msg_sn:     .asciiz ", SN = "
msg_nl:     .asciiz "\n"

.text
.globl main

main:
	# Test 1: PENO(arr1, 10)
	la   $a0, arr1
	li   $a1, 10
	jal  PENO

	# Save returned sums before using syscalls
	move $s0, $v0
	move $s1, $v1

	# Print: Case1: SP = <v0>, SN = <v1>
	li   $v0, 4
	la   $a0, msg_case1
	syscall

	li   $v0, 1
	move $a0, $s0
	syscall

	li   $v0, 4
	la   $a0, msg_sn
	syscall

	li   $v0, 1
	move $a0, $s1
	syscall

	li   $v0, 4
	la   $a0, msg_nl
	syscall

	# Test 2: PENO(arr2, 10)
	la   $a0, arr2
	li   $a1, 10
	jal  PENO

	# Save returned sums before using syscalls
	move $s0, $v0
	move $s1, $v1

	# Print: Case2: SP = <v0>, SN = <v1>
	li   $v0, 4
	la   $a0, msg_case2
	syscall

	li   $v0, 1
	move $a0, $s0
	syscall

	li   $v0, 4
	la   $a0, msg_sn
	syscall

	li   $v0, 1
	move $a0, $s1
	syscall

	li   $v0, 4
	la   $a0, msg_nl
	syscall

	# Exit
	li   $v0, 10
	syscall


# ------------------------------------------------------------
# PENO(&X, N, SP, SN)
# Input : $a0 = base address of int array X
#         $a1 = length N
# Output: $v0 = sum of positive odd numbers (SP)
#         $v1 = sum of negative even numbers (SN)
# ------------------------------------------------------------
PENO:
	move $v0, $zero       # SP = 0
	move $v1, $zero       # SN = 0

	move $t0, $zero       # i = 0
	move $t1, $a0         # current element pointer

loop:
	beq  $t0, $a1, done

	lw   $t2, 0($t1)      # t2 = X[i]

	# If X[i] > 0 and odd, add to SP
	blez $t2, check_neg_even
	andi $t3, $t2, 1
	bne  $t3, 1, next
	addu $v0, $v0, $t2
	j    next

check_neg_even:
	# If X[i] < 0 and even, add to SN
	bgez $t2, next
	andi $t3, $t2, 1
	bne  $t3, $zero, next
	addu $v1, $v1, $t2

next:
	addi $t0, $t0, 1
	addi $t1, $t1, 4
	j    loop

done:
	jr   $ra