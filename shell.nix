with import <nixpkgs> {};
  mkShell {
    packages = [
      arduino-cli

      nixd
      alejandra

      (python3.withPackages (packages:
        with packages; [
          matplotlib
          numpy
          sympy
          scipy
          pandas
          seaborn

          # python3 -m jupyterlab
          jupyterlab
          ipykernel
          ipywidgets
          ipython
        ]))

      typst
      tinymist
      typstyle
    ];
  }
