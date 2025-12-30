# Repository Guidelines

## Project Structure & Module Organization
- `os/` holds the kernel, network stacks, and shared libraries; `arch/` contains MCU/board ports and drivers; `examples/` groups runnable demos by feature; `tests/` contains numbered regression suites (compile, simulation, Renode, native runs); `tools/` includes simulators and code-style helpers; `doc/` stores contributor and platform docs. Build helpers live in the root `Makefile.*`.

## Build, Test, and Development Commands
- Build a sample for native simulation: `make -C examples/hello-world TARGET=native` (produces `hello-world.native` in the example folder).
- Discover supported targets/boards: `make targets` and `make boards TARGET=<target>`.
- Clean a build directory: `make -C examples/hello-world TARGET=native clean`.
- Run the aggregated regression suite locally: `make -C tests` (creates `tests/summary` from all numbered suites). Static analysis: `make -C tests scan-build`.
- Firmware profiling helpers (per `Makefile.help`): append `.flashprof` or `.ramprof` to a firmware target to inspect ROM/RAM use.

## Coding Style & Naming Conventions
- C code follows the C11 subset supported by GCC 4.7; prefer POSIX APIs on the native target. File names use lowercase-with-dashes; functions/vars use `snake_case` with module prefixes for exported symbols; macros are `UPPER_SNAKE`; config options start with `<module>_CONF_`.
- Use the Uncrustify helpers under `tools/code-style`: check with `./tools/code-style/uncrustify-check-style.sh <file>`; apply fixes with `./tools/code-style/uncrustify-fix-style.sh <file ...>`. Doxygen comments are expected for public APIs.

## Testing Guidelines
- Place new tests under the matching numbered suite in `tests/`; mimic existing `NN-name` folders to keep summaries working. For platform ports, add compile regressions so CI covers your target.
- Before opening a PR, run the subset relevant to your change (e.g., `make -C tests/08-native-runs summary` for native apps) and ensure GitHub Actions passes.

## Commit & Pull Request Guidelines
- Keep commits focused and use short, imperative titles (e.g., “Add RPL-lite hook”). Rebase onto the target branch before pushing.
- Target branches: features → `develop`; minor fixes → `release-X.Y` if present, else `develop`; hotfixes → `master` (mirrored back to `develop`). Avoid dead/commented code.
- PRs must explain scope, list test results (local + CI), call out docs/wiki updates, and note any hardware coverage or limitations.

## Security & Configuration Tips
- Security fixes land on `develop` and may be hotfixed to the latest release; older releases are unsupported. Report vulnerabilities to `security@contiki-ng.org` (see `SECURITY.md`).
- Do not commit secrets or proprietary blobs; prefer board configs under `arch/` with documented options rather than ad-hoc tweaks.
