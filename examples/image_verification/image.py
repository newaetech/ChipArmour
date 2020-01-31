import chipwhisperer as cw

scope = cw.scope()
target = cw.target(scope)
scope.default_setup()

prog = cw.programmers.STM32FProgrammer
cw.program_target(scope, prog, "image-demo-CWLITEARM.hex")

line = ""
while True:
    print(target.read(), end="")