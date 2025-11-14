{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    gcc
    gnumake
    raylib
    bear
    cmake
    clang-tools
  ];
}
