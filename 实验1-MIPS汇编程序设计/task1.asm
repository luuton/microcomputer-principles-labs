.data
arr1:       .word 1, -4, 8, -9, 5, 6, -10, 19, 22, 23
arr2:       .word 121, -124, 138, -199, 255, 2566, -1034, 1019, 2032, 2033


msg_case1:  .asciiz "Case1 SP = "
msg_case2:  .asciiz "Case2 SP = "
msg_sn:     .asciiz ", SN = "
msg_nl:     .asciiz "\n"

.text
.globl main

main:
	# Test 1 PENO(arr1, 10)
	la   $a0, arr1
	li   $a1, 10
	jal  PENO

	# Save returned sums before using syscalls
	move $s0, $v0
	move $s1, $v1

	# Printf Case1 SP = <v0>, SN = <v1>
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

	# Test 2 PENO(arr2, 10)
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

# int PENO(int *X, int N, int SP, int SN) {
#     int SP = 0    // 正奇数和
#     int SN = 0    // 负偶数和
#     for (int i = 0 i < N i++) {
#         if (X[i] > 0 && X[i] % 2 == 1) {
#             SP += X[i]
#         } else if (X[i] < 0 && X[i] % 2 == 0) {
#             SN += X[i]
#         }
#     }
#     return 0
# }


PENO:
    li $v0, 0  #SP = 0
    li $v1, 0  #SN = 0

    li $t0, 0  #int i = 0
    move $t3, $a0

loop:
    beq $t0, $a1, done #if i==N goto done

    # if (X[i] > 0 && X[i] % 2 == 1)
    lw $t2, 0($t3)     #$t2 = X[i]
    blez $t2, else     #if($t2 <= 0) goto else
    andi $t4, $t2, 1    #$t2 and 1 = 1,odd
    beq $t4, $zero, next#t4 = 0 goto next(not odd)
    add $v0, $v0, $t2  #odd, sp = sp + X[i]
    j next

else:
    # else if (X[i] < 0 && X[i] % 2 == 0)
    beq $t2, $zero, next   #check X[i] = 0
    andi $t5, $t2, 1    #$t2 and 1 = 1,odd
    beq $t5, 1, next    #if $t5 == 1, goto next(X is odd)
    add $v1, $v1, $t2  #sn = sn + X[i]

next:
    # i++
    addi $t0, $t0, 1
    addi $t3, $t3, 4
    j loop

done:
    jr $ra
