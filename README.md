# Hardfault Recovery for Cortex M4
### I plan to add similar files for Cortex M33
Example "hardfault_handler.c" and "startup.s" provided

To Use:
- Include c file in build
  - Assuming other fault managers not enabled (only hardfault enabled)
- Set define depending on your goal:
  - Try to Recover from hardfault (if possible):
    - Will return to next instruction (if possible) upon a hardfault
    - ```#define TRY_RECOVERY ```
   - Display stack in debugger or serial console:
      - ```#define DUMP_STACK ```
      - Assuming printf is retargeted correctly

# Sources: 
- https://www.embedded.com/debugging-hard-faults-in-arm-cortex-m0-based-socs/
- https://interrupt.memfault.com/blog/cortex-m-fault-debug#registers-prior-to-exception
- http://www.keil.com/appnotes/files/apnt209.pdf

