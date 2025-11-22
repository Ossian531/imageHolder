{
  description = "ImageHolder - SDL2 image viewer application";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages = {
          default = pkgs.stdenv.mkDerivation {
            pname = "imageholder";
            version = "0.1.0";

            src = ./.;

            nativeBuildInputs = with pkgs; [
              gcc
              pkg-config
            ];

            buildInputs = with pkgs; [
              SDL2
              SDL2_image
            ];

            buildPhase = ''
              g++ -o imageHolder imageHolder.cpp $(pkg-config --cflags --libs sdl2 SDL2_image)
            '';

            installPhase = ''
              mkdir -p $out/bin
              cp imageHolder $out/bin/
            '';

            meta = with pkgs.lib; {
              description = "SDL2-based image viewer that displays screenshots";
              license = licenses.mit;
              platforms = platforms.linux;
            };
          };
        };

        # Development shell with all necessary dependencies
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            pkg-config
            SDL2
            SDL2_image
          ];

          shellHook = ''
            echo "ImageHolder development environment"
            echo "Build with: g++ -o imageHolder imageHolder.cpp \$(pkg-config --cflags --libs sdl2 SDL2_image)"
            echo "Or run: nix build"
          '';
        };

        # Make the app runnable with 'nix run'
        apps.default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/imageHolder";
        };
      }
    );
}
