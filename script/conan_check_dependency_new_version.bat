@ECHO OFF
SETLOCAL

cd /d "%~dp0\.."

IF "%VTX_CONAN_GRAPH_PATH%"=="" SET "VTX_CONAN_GRAPH_PATH=dev"
IF "%VTX_CONAN_REMOTE%"=="" SET "VTX_CONAN_REMOTE=conancenter"

SET "GRAPH_JSON=%TEMP%\vtx_conan_graph_%RANDOM%%RANDOM%.json"

ECHO Building Conan dependency graph from "%VTX_CONAN_GRAPH_PATH%"...
conan graph info "%VTX_CONAN_GRAPH_PATH%" %VTX_CONAN_GRAPH_ARGS% --format=json > "%GRAPH_JSON%"
IF ERRORLEVEL 1 GOTO :graph_error

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$ErrorActionPreference = 'Continue'; " ^
  "$graph = Get-Content -Raw '%GRAPH_JSON%' | ConvertFrom-Json; " ^
  "$nodes = $graph.graph.nodes.PSObject.Properties; " ^
  "$root = ($nodes | Where-Object { $_.Name -eq '0' } | Select-Object -First 1).Value; " ^
  "if (-not $root) { throw 'Unable to find root node in Conan graph.' } " ^
  "$refs = @(); " ^
  "if ($root.dependencies) { " ^
  "  $refs = @($root.dependencies.PSObject.Properties.Value | Where-Object { $_.direct -and $_.ref } | ForEach-Object { $_.ref -replace '#.*$', '' }); " ^
  "} " ^
  "$refs = @($refs | Where-Object { $_ } | Sort-Object -Unique); " ^
  "if ($refs.Count -eq 0) { throw 'No direct dependency found in Conan graph.' } " ^
  "function Get-VersionScheme([string]$version) { " ^
  "  if ($version -match '^cci\.\d+$') { return 'cci' } " ^
  "  if ($version -match '^\d{8}$') { return 'date' } " ^
  "  if ($version -match '^\d') { return 'numeric' } " ^
  "  return 'other'; " ^
  "} " ^
  "function Compare-ConanVersion([string]$left, [string]$right) { " ^
  "  if ($left -eq $right) { return 0 } " ^
  "  $leftTokens = @([regex]::Matches($left, '\d+|[A-Za-z]+') | ForEach-Object { $_.Value }); " ^
  "  $rightTokens = @([regex]::Matches($right, '\d+|[A-Za-z]+') | ForEach-Object { $_.Value }); " ^
  "  $max = [Math]::Max($leftTokens.Count, $rightTokens.Count); " ^
  "  for ($i = 0; $i -lt $max; ++$i) { " ^
  "    $leftToken = if ($i -lt $leftTokens.Count) { $leftTokens[$i] } else { '0' }; " ^
  "    $rightToken = if ($i -lt $rightTokens.Count) { $rightTokens[$i] } else { '0' }; " ^
  "    $leftIsNumber = $leftToken -match '^\d+$'; " ^
  "    $rightIsNumber = $rightToken -match '^\d+$'; " ^
  "    if ($leftIsNumber -and $rightIsNumber) { " ^
  "      $leftNumber = [int64]$leftToken; " ^
  "      $rightNumber = [int64]$rightToken; " ^
  "      if ($leftNumber -lt $rightNumber) { return -1 } " ^
  "      if ($leftNumber -gt $rightNumber) { return 1 } " ^
  "    } else { " ^
  "      $result = [string]::Compare($leftToken, $rightToken, [StringComparison]::OrdinalIgnoreCase); " ^
  "      if ($result -ne 0) { return $result } " ^
  "    } " ^
  "  } " ^
  "  return 0; " ^
  "} " ^
  "Write-Host ('Checking ' + $refs.Count + ' direct dependencies on %VTX_CONAN_REMOTE%...'); " ^
  "$hasNewer = $false; " ^
  "foreach ($ref in $refs) { " ^
  "  $name, $current = $ref -split '/', 2; " ^
  "  $listJson = conan list ($name + '/*') -r=%VTX_CONAN_REMOTE% --format=json 2>$null; " ^
  "  if ($LASTEXITCODE -ne 0 -or -not $listJson) { Write-Host ('[?] ' + $ref + ' -> list failed'); continue } " ^
  "  $list = $listJson | ConvertFrom-Json; " ^
  "  $remote = $list.PSObject.Properties | Select-Object -First 1; " ^
  "  if (-not $remote -or $remote.Value.PSObject.Properties['error']) { Write-Host ('[?] ' + $ref + ' -> list failed'); continue } " ^
  "  $currentScheme = Get-VersionScheme $current; " ^
  "  $versions = @($remote.Value.PSObject.Properties.Name | ForEach-Object { ($_ -split '/', 2)[1] } | Where-Object { $_ -and (Get-VersionScheme $_) -eq $currentScheme }); " ^
  "  if ($versions.Count -eq 0) { Write-Host ('[?] ' + $ref + ' -> no compatible version'); continue } " ^
  "  $latest = $current; " ^
  "  foreach ($version in $versions) { " ^
  "    if ((Compare-ConanVersion $latest $version) -lt 0) { $latest = $version } " ^
  "  } " ^
  "  if ($latest -ne $current) { " ^
  "    $hasNewer = $true; " ^
  "    Write-Host ('[+] ' + $ref + ' -> ' + $name + '/' + $latest); " ^
  "  } else { " ^
  "    Write-Host ('[=] ' + $ref); " ^
  "  } " ^
  "} " ^
  "if ($hasNewer) { exit 2 } " ^
  "exit 0"

SET "CHECK_RESULT=%ERRORLEVEL%"
GOTO :cleanup

:graph_error
ECHO Failed to build Conan graph.
GOTO :cleanup_error

:cleanup
DEL "%GRAPH_JSON%" >NUL 2>NUL
EXIT /B %CHECK_RESULT%

:cleanup_error
DEL "%GRAPH_JSON%" >NUL 2>NUL
EXIT /B 1
