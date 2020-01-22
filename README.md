# ChipArmour™

ChipArmour™ is an open-source library that helps developers build secure embedded devices by considering advanced attacks like fault injection. ChipArmour™ uses a permissive Apache license for the source code and does not require any per-device fees, even if using commercially. Commercially supported packages and builds are optionally available.

ChipArmour™ is linked to the ChipWhisperer® project, which was the first complete open-source toolchain for power analysis and fault injection. The ChipArmour™ libraries are automatically tested on a hardware testbench that validates the design decisions in real-life environments, by attempting fault injection attacks on the library.

ChipArmour™ is designed to be a "minimal pain" solution for existing projects, without switching languages, compilers, etc. Currently it works with GCC (IAR should work with little effort). While it mostly focuses on FI prevention, other attacks (ROP, etc) will eventually be integrated.

## Example Projects and Templates

Besides the actual fault-injection resistant code being used, the memory protection units and other features of the microcontroller must be configured correctly. ChipArmour contains examples of this for various devices, and should be used as a template when starting your new projects.

## Validation Environment

If you inspect many projects, you'll find *assumed* side-channel power or fault injection countermeasures. These are tricks developers have inserted into the code, but typically do not validate them in real hardware. Or they *do* validate them, but only do it once and do not check they remained active. Poor fault models, and compilers later changing the resulting assembly frequently result in those tricks being much easier to bypass than you expect from looking at the source code.

For this reason ChipArmour has a hardware test bench. Currently this testing is done on three platforms: SAML11 (Cortex M23 core), STM32F3 (Cortex M4 core), and STM32F0 (Cortex M0 core). The SAML11 is a good platform as it appears highly vulnerable to voltage fault injection, so makes a good target device as a worst-case. The ChipWhisperer platform supports many other devices, and other architectures can easily be added. Validated libraries are tested on specific devices for the most accurate fault injection resistance guarantees.

### Use of Binary Libraries vs. Source Code

ChipArmour can be used either in the raw source code (FOSS, Apache licence), or as a binary library (commercial licence with support).

The binary library also includes optimized assembly-language versions of the library, which maintain the fault resistance with considerably less overhead. Using the binary library ensures the validated object code has been used in the final linking stage.

The Apache licence does allow you to build your own binary libraries of course. Note that parts of the validation environment are not part of the FOSS ChipArmour project.

## Status of Library

The library itself is in **early alpha**. Using the API now makes it easier to switch the underlying code around as improvements happen, and you will automatically receive better protection against fault injection attack threats.

There may yet be large changes in the API, as we discover more effective methods of shaping our FI resistance.

## Legal and Other Notes


### Contributor Agreement

ChipArmour is open-source licensed. We do not require any contributor agreement to accept your pull requests, but note your changes will be bound by terms of the Apache license (meaning they can be freely used in commercial products without modifications to them being released). If you are an embedded developer, you can freely contribute back changes. We are working on how to recognize contributors, as many contributors are independent consultants that commercial users of ChipArmour may like to engage with. While NewAE Technology also offers such support, we do not intend to discourage others in the community from offering their expertise (even if in competition with NewAE).

### Security Disclosure, Bug Bounty

We appreciate any security disclosures or notes. Currently the library is in alpha and many changes are expected, so just ask you to open github issues for now.

There is currently no bug bounty program or similar. We hope to offer/track more of these in the future.

### ChipArmour vs. ChipArmor

ChipArmour started in Canada, where 'armour' is the preferred spelling. It's easy to remember by the "u" by always thinking that "ChipArmour needs YOU (to be using it)".

### Trademark Disclaimers

ChipArmour and ChipArmor are trademarks of NewAE Technology Inc. ChipWhiserer is a registered trademark of NewAE Technology Inc.
