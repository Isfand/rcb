{
  lib,
  stdenv,
  build2,
  fetchFromGitHub,
  sqlite,
  enableShared ? !stdenv.hostPlatform.isStatic,
  enableStatic ? !enableShared,
}:

stdenv.mkDerivation (finalAttrs: let owner = "Isfand"; in {
  pname = "rcb";
  version = "0.14.0";

  outputs = [
    "out"
    "doc"
  ];

  src = fetchFromGitHub {
    inherit owner;
    repo  = finalAttrs.pname;
    rev = "v${finalAttrs.version}";
    hash = "sha256-bvtd0fbu4Ytf9oZnOQ0rBmW/qOGn60GNMnRmQSPjZKs=";
  };

  sourceRoot = "source/rcb";
  nativeBuildInputs = [ build2 ];
  buildInputs = [ sqlite ];
  build2ConfigureFlags = [
    "config.bin.lib=${build2.configSharedStatic enableShared enableStatic}"
  ];


  preCheck = ''
    export RCB_DIR=$(mktemp -d)
  '';
  doCheck = true;

  meta = {
    description = "File recycling utility for the terminal";
    longDescription = ''
      File recycling utility for the terminal that can delete, restore, erase, list and validate files.
    '';

    homepage = "https://github.com/Isfand/rcb";
    changelog = "https://github.com/Isfand/rcb/blob/master/rcb/docs/CHANGELOG.txt";
    license = lib.licenses.epl20;
    #maintainers = with lib.maintainers; [ inherit owner ];
    platforms = lib.platforms.all;
	mainProgram = finalAttrs.pname;
  };
})
