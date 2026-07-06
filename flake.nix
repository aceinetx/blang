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
            xmake b
            xmake i -o .
          '';

          buildInputs = with pkgs; [
            gcc
            xmake
            cmake
            ninja
            pkg-config
            llvm
            fmt
            bison
            m4
            openssl
            curl
            jsoncpp
            argtable
            catch2
          ];

          installPhase = ''
            mkdir -p $out/bin
            cp bin/blang $out/bin/
          '';
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            xmake
            cmake
            ninja
            pkg-config
            llvm
            fmt
            bison
            m4
            openssl
            curl
            jsoncpp
            argtable
            catch2
          ];
        };
      }
    );
}
