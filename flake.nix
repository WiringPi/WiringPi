{
  description = "A flake for building the gpio binary needed by wiringPi library";

  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs/nixos-22.11;
  };

  outputs = { self, nixpkgs }: {
    inherit nixpkgs;

    defaultPackage.aarch64-linux=
      with import nixpkgs { 
        system = "aarch64-linux"; 
      };

      gcc12Stdenv.mkDerivation {
        name = "acbcl_tests";
        src = self;
        phases = [ "unpackPhase" "buildPhase" "installPhase" ];
        buildPhase = ''
          mkdir build_outputs && cd build_outputs
          cmake .. -GNinja
          cmake --build . --verbose
        '';

        installPhase = ''
        mkdir -p $out/bin
        mkdir -p $out/lib
        cp gpio/gpio $out/bin
        cp wiringPi/libwiringPi.a $out/lib
        '';

        buildInputs = with nixpkgs; [
          cmake
          ninja
          git
        ];

        nativeBuildInputs = with nixpkgs; [ pkg-config libxcrypt ];
      };

  };
}
