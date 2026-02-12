with import <nixpkgs> {};
  mkShell {
    packages = [
      arduino-cli

      nixd
      alejandra
    ];
  }
