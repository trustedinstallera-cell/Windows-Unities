# 设置控制台窗口大小
$width = 50
$height = 5

# 调整缓冲区大小
$bufferWidth = 60
$bufferHeight = $height

# 设置窗口大小
$host.UI.RawUI.WindowSize = New-Object System.Management.Automation.Host.Size($width, $height)
$host.UI.RawUI.BufferSize = New-Object System.Management.Automation.Host.Size($bufferWidth, $bufferHeight)

Add-Type @"
    using System;
    using System.Runtime.InteropServices;
    public class WindowTopmost {
        [DllImport("user32.dll")]
        public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);
        
        [DllImport("user32.dll")]
        public static extern IntPtr GetForegroundWindow();
        
        public static void SetTopmost() {
            IntPtr hWnd = GetForegroundWindow();
            SetWindowPos(hWnd, (IntPtr)(-1), 0, 0, 0, 0, 0x0002 | 0x0001);
        }
    }
"@

[WindowTopmost]::SetTopmost()

# 显示静态文本（带冒号对齐）
Clear-Host
Write-Host ("{0,-15} " -f "File Count:")
Write-Host ("{0,-15} " -f "Total Chars:")
Write-Host ("{0,-15} " -f "Total Size:")
Write-Host ("{0,-15} " -f "Line Count:")

while($true){    
    # 获取文件列表
    $files = Get-ChildItem -Path .\* -Include *.cpp, *.h -Recurse

    # 计算各项指标
    $fileCount = $files.Count
    $totalSize = ($files | Measure-Object -Property Length -Sum).Sum
    
    # 统计总字符数
    $totalChars = 0
    foreach ($file in $files) {
        $charsInFile = (Get-Content -Path $file.FullName -ErrorAction SilentlyContinue | Measure-Object -Character).Characters
        $totalChars += $charsInFile
    }
    
    # 统计行数
    $lines = 0
    if ($files.Count -gt 0) {
        $lines = ($files | Get-Content -ErrorAction SilentlyContinue | Measure-Object -Line).Lines
    }

    # 格式化文件大小
    if ($totalSize -gt 1GB) {
        $sizeDisplay = "{0:N2} GB" -f ($totalSize / 1GB)
    } elseif ($totalSize -gt 1MB) {
        $sizeDisplay = "{0:N2} MB" -f ($totalSize / 1MB)
    } elseif ($totalSize -gt 1KB) {
        $sizeDisplay = "{0:N2} KB" -f ($totalSize / 1KB)
    } else {
        $sizeDisplay = "$totalSize B"
    }

    # 更新数值（调整列位置到15）
    [System.Console]::SetCursorPosition(15, 0)
    [System.Console]::ForegroundColor = [System.ConsoleColor]::Yellow
    Write-Host ("{0,-40}" -f $fileCount) -NoNewline
    
    [System.Console]::SetCursorPosition(15, 1)
    [System.Console]::ForegroundColor = [System.ConsoleColor]::Green
    Write-Host ("{0,-40}" -f $totalChars) -NoNewline
    
    [System.Console]::SetCursorPosition(15, 2)
    [System.Console]::ForegroundColor = [System.ConsoleColor]::Magenta
    Write-Host ("{0,-40}" -f "$sizeDisplay ($totalSize bytes)") -NoNewline
    
    [System.Console]::SetCursorPosition(15, 3)
    [System.Console]::ForegroundColor = [System.ConsoleColor]::Cyan
    Write-Host ("{0,-40}" -f $lines) -NoNewline
    
    # 重置颜色
    [System.Console]::ResetColor()
    
    # 设置窗口标题
    $host.UI.RawUI.WindowTitle = "$fileCount files | $totalChars chars | $sizeDisplay"
    
    Start-Sleep -Seconds 1
}
