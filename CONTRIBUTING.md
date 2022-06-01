# Contributing

**Open OCPP** welcomes contributions. When contributing, please follow the code below.

## Coding rules

* The **.clang-format** file at the root of the source tree must not be modified (or **after** having a discussion between all the contributors)
* The code must formatted using the above mentionned file with a clang-format compliant tools (ex: Visual Studio Code)
* Every interface/class/method must be documented using the [Doxygen](https://www.doxygen.nl/) format
* Use of smart pointers for memory allocation is greatly recommended
* Use of C/C++ macros is discouraged
* Keep code simple to understand and don't be afraid to add comments!

## Issues

Feel free to submit issues and enhancement requests.

Please help us by providing minimal reproducible examples, because source code is easier to let other people understand what happens. For crash problems on certain platforms, please bring stack dump content with the detail of the OS, compiler, etc.

Please try breakpoint debugging first, tell us what you found, see if we can start exploring based on more information been prepared.

## Workflow

Follow the "fork-and-pull" Git workflow :

* Fork the repo on GitHub
* Clone the project to your own machine
* Checkout a new branch on your fork, start developing on the branch
* Test the change before commit, Make sure the changes pass all the tests, please add test case for each new feature or bug-fix if needed.
* Commit changes to your own branch
* Push your work back up to your fork
* Submit a Pull request so that we can review your changes

**Be sure to merge the latest from "upstream" before making a pull request!**
