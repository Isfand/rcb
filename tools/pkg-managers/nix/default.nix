{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation rec {
  pname = "trash";
  version = "0.9.4";

  src = pkgs.fetchFromGitHub {
    owner = "Isfand";
    repo = "trash";
    rev = "v0.9.4";
    sha256 = "sha256-placeholder"; # Replace with the actual sha256 hash
  };

  nativeBuildInputs = [ pkgs.build2 ];

  buildPhase = ''
    bdep init -C @x64-posix-llvm-release cc config.config.load=tools/build-configs/posix-llvm-release.config && bdep sync "?sys:libsqlite3"
  '';

  installPhase = ''
    b install config.cxx.loptions="-L/usr/local/lib -Wl,-rpath,/usr/local/lib" config.install.root="/usr/local" config.install.sudo="sudo"
  '';

  meta = with pkgs.lib; {
    description = "File trashing utility for the terminal";
    homepage = "https://github.com/Isfand/trash";
    license = Eclipse Public license 2.0;
    maintainers = with maintainers; [ Isfand ];
  };
}
