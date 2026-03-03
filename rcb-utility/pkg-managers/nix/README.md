#WIP

#Info
This local port requires build2 hooks from the nix channels, which are automatically added or manually from the official git repo. (nixpkgs/pkgs/by-name/bu/build2-bootstrap/setup-hook.sh
)
Assuming you already have nix installed and the nix channel(s) added. E.G:
nix-channel --add https://nixos.org/channels/nixpkgs-unstable unstable
nix-channel --add https://nixos.org/channels/nixos-24.11 nixpkgs
nix-channel --update

<nixpkgs-location> is user dependant. Currently assuming it's in /opt/ports/nixpkgs for the manual instructions.

#Optional: Nix itself does not require this. You can just skip to #Build but using relative instructions instead.
mkdir -p /opt/ports
cd /opt/ports
git clone https://github.com/NixOS/nixpkgs.git
cp -r nixpkgs/pkgs/by-name/rc/rcb /opt/ports/nixpkgs/pkgs/by-name/rc/rcb

#Test
nix-build --dry-run -E 'with import /opt/ports/nixpkgs {}; callPackage /opt/ports/nixpkgs/pkgs/by-name/rc/rcb/package.nix {}'

#Build
nix-build -E 'with import /opt/ports/nixpkgs {}; callPackage /opt/ports/nixpkgs/pkgs/by-name/rc/rcb/package.nix {}'

#Install
nix-env -i -E 'with import /opt/ports/nixpkgs {}; callPackage /opt/ports/nixpkgs/pkgs/by-name/rc/rcb/package.nix {}'