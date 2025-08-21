# Mile.Cirno Release Notes

**Mile.Cirno 1.1 (1.1.750.0)**

- Provide read-write client support for 9P2000.L protocol.
- Improve the performance.
- Add get volume information support.
- Improve get file information support.
- Make maximum message size negotiable.
- Fix several logic errors.
- Improve Mile.Cirno.Protocol definitions based on Windows Subsystem for Linux
  source code.
- Add the project icon.
- Fix the source code file header comment format.
- Update Mile.Dokany to 1.1.662.
- Update Mile.Project.Configurations to 1.0.1622.
- Update Mile.Windows.Helpers to 1.0.952.
- Update Mile.Windows.UniCrt to 1.2.410 to fix the missing symbols issue at
  runtime for the x86-32 target.

**Mile.Cirno 1.0 (1.0.501.0)**

- Initial release.
- Provide read only client support for 9P2000.L protocol, based on
  [Mile.Dokany](https://github.com/ProjectMile/Mile.Dokany), the customized
  version of [Dokany](https://github.com/dokan-dev/dokany) which user-mode
  library is specialized for NuGet integration and optimized for binary size.
- Support 9p transport over TCP or Hyper-V Socket.
- Provide NanaBox EnableHostDriverStore integration mode.
