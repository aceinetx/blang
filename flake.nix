{
  description = "blang flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          name = "blang";
          version = "1.0.0";
          src = self;

          buildPhase = ''
            zig build
          '';

          buildInputs = with pkgs; [
            gcc
            zig_0_16
            pkg-config
            llvm
          ];

          installPhase = ''
            mkdir -p $out/bin
            cp zig-out/bin/blang $out/bin/
          '';
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            zig_0_16
            pkg-config
            llvm
          ];
        };
      }
    );
}
