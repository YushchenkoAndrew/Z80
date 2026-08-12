{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [ gnumake gcc emscripten pkg-config gdb tree bear clang-tools ];
  buildInputs = with pkgs; [ libX11 libGL libpng alsa-lib lua5_4 ]; # Everything -lX11 -lGL -lpng -lasound -llua needs

  shellHook = ''
    # nixpkgs ships emscripten's port cache read-only in /nix/store, but em++
    # must write to it to build ports (-s USE_LIBPNG=1, USE_PTHREADS, ...).
    # export EM_CACHE="$PWD/.emscripten_cache"
    # if [ ! -d "$EM_CACHE" ]; then
    #   echo "seeding writable emscripten cache in $EM_CACHE"
    #   mkdir -p "$EM_CACHE"
    #   cp -r ${pkgs.emscripten}/share/emscripten/cache/. "$EM_CACHE"/ || true
    #   chmod -R u+w "$EM_CACHE"
    # fi

    # matches `make run`, for the vendored liblua54.so
    export LD_LIBRARY_PATH="$PWD''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

    # --preload-file assets/ fails hard if the dir is missing
    make init >/dev/null 2>&1 || true

    echo "C++ Go development environment!"
  '';
}
