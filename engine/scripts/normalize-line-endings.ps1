# Normalize line endings to CRLF for Windows development
# Run this from the project root directory

Write-Host "Normalizing line endings to CRLF..." -ForegroundColor Cyan

# Get all source files
$patterns = @(
    "*.cpp",
    "*.h",
    "*.hpp",
    "*.c",
    "*.cc",
    "*.cxx",
    "*.cmake",
    "CMakeLists.txt",
    "*.md",
    "*.txt",
    "*.json",
    "*.yml",
    "*.yaml"
)

$count = 0
$skipped = 0

foreach ($pattern in $patterns)
{
    $files = Get-ChildItem -Path .. -Recurse -Include $pattern -File | Where-Object {
        # Skip vendor directory and build directories
        $_.FullName -notmatch '\\vendor\\' -and
        $_.FullName -notmatch '\\build\\' -and
        $_.FullName -notmatch '\\out\\' -and
        $_.FullName -notmatch '\\.git\\' -and
        $_.FullName -notmatch '\\vcpkg_installed\\'
    }

    foreach ($file in $files)
    {
        try
        {
            # Read file
            $content = Get-Content -Path $file.FullName -Raw -ErrorAction Stop
            
            if ($null -eq $content -or $content.Length -eq 0)
            {
                Write-Host "  Skipping empty file: $($file.Name)" -ForegroundColor Yellow
                $skipped++
                continue
            }

            # Check current line ending
            $hasCRLF = $content -match "`r`n"
            $hasLF = $content -match "(?<!\r)`n"

            if ($hasCRLF -and -not $hasLF)
            {
                Write-Host "  Already CRLF: $($file.Name)" -ForegroundColor DarkGray
                $skipped++
                continue
            }

            # Convert to CRLF
            # First normalize to LF, then to CRLF (handles mixed line endings)
            $normalized = $content -replace "`r`n", "`n"  # Remove existing CRLF
            $normalized = $normalized -replace "`r", "`n" # Remove stray CR
            $normalized = $normalized -replace "`n", "`r`n" # Add CRLF
            
            # Write back
            [System.IO.File]::WriteAllText($file.FullName, $normalized)
            
            Write-Host "  Converted: $($file.Name)" -ForegroundColor Green
            $count++
        }
        catch
        {
            Write-Host "  Error processing $($file.Name): $($_.Exception.Message)" -ForegroundColor Red
        }
    }
}

Write-Host ""
Write-Host "Complete!" -ForegroundColor Green
Write-Host "  Files converted: $count" -ForegroundColor Cyan
Write-Host "  Files skipped: $skipped" -ForegroundColor Yellow
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "  1. Review changes: git status" -ForegroundColor White
Write-Host "  2. Check line endings: git ls-files --eol" -ForegroundColor White
Write-Host "  3. Commit if satisfied: git commit -m 'Normalize to CRLF'" -ForegroundColor White
