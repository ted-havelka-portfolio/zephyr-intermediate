# Iomico Zephyr Intermediate Course - Lecture 7 Security

2026-11-27

Following recorded lecture and written task 7 instructions, student Ted applied
Memory Protection Unit (MPU) enabling Kconfigs to the QEMU based sample app.
Zephyr documentation provides introductory, general information about MPU
facilities in microcontrollers. An article posted by Wadix Technologies fills a
number of details and provides useful examples.

# Lecture 7, task 7 Questions answered:

1. (Q1) The best I can say, not having written programs which secure their
   resources with public/private key pairs, is that some part of the app not
   given in the l7 homework is reponsible for any start up time authentication,
   where credentials from an outside calling process would be compared against
   the private key. Assuming an attacker has access to the firmware image, say
   by buying a sample device and reading out the firmware, the product at large
   can still be safe so long as unique certificates are provisioned with each
   device. That way the attacker won't know the private keys on devices
   elsewhere, and with memory protection a parser thread won't be directable to
   read any private key data, via a malicious command.

1. (Q2) In the L7 application the parser thread should run in user mode, and the
   private key data should be in a kernel memory parition or space, not any user
   memory partition.

1. (Q3) One feature of Zephyr which provides memory protection is its support
   for hardware memory protection units. Zephyr 4.4.0 for example provides
   several MPU related macros, and an API to configure memory partitions. These
   partitions can be assigned to the stacks of specific threads, and possibly
   other programmatic elements, to protect against security-wise, out of bounds
   memory read and write operations. This is explained in good detail in the
   Wadix Technologies article, link given among references noted in this readme.

# References

- https://docs.zephyrproject.org/latest/security/secure-coding.html

- https://docs.zephyrproject.org/latest/samples/userspace/shared_mem/README.html

- https://docs.zephyrproject.org/latest/doxygen/html/group__mem__domain__apis__app.html#ga8fd691f9c9007ec80432c19f7ce84881

- https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/userspace/shared_mem/src/main.h

- https://www.wadixtech.com/blog/zephyr-rtos-userspace-cortex-m-mpu-isolation
