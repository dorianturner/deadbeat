{
  description = "Development shell for Deadbeat";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        cppTools = with pkgs; [
          libgcc
          gnumake
          git
          portaudio
          libsndfile
          lame
          clang-tools
          gdb
          cmake
          pkg-config
        ];
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = cppTools;

          shellHook = ''
            echo "Deadbeat C++ dev shell ready"
          '';
        };
      });
}

