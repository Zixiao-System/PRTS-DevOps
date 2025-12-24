<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import { Terminal } from '@xterm/xterm'
import { FitAddon } from '@xterm/addon-fit'
import { WebLinksAddon } from '@xterm/addon-web-links'
import '@xterm/xterm/css/xterm.css'

const terminalContainer = ref<HTMLElement | null>(null)
let terminal: Terminal | null = null
let fitAddon: FitAddon | null = null

const welcomeMessage = `
\x1b[1;36m╔══════════════════════════════════════════════════════════╗
║                                                              ║
║   \x1b[1;33m██╗      ██████╗  ██████╗  ██████╗ ███████╗\x1b[1;36m               ║
║   \x1b[1;33m██║     ██╔═══██╗██╔════╝ ██╔═══██╗██╔════╝\x1b[1;36m               ║
║   \x1b[1;33m██║     ██║   ██║██║  ███╗██║   ██║███████╗\x1b[1;36m               ║
║   \x1b[1;33m██║     ██║   ██║██║   ██║██║   ██║╚════██║\x1b[1;36m               ║
║   \x1b[1;33m███████╗╚██████╔╝╚██████╔╝╚██████╔╝███████║\x1b[1;36m               ║
║   \x1b[1;33m╚══════╝ ╚═════╝  ╚═════╝  ╚═════╝ ╚══════╝\x1b[1;36m               ║
║                                                              ║
║   \x1b[1;37mLogos IDE - PRTS DevOps Platform\x1b[1;36m                          ║
║   \x1b[0;37m智慧与洞察的化身\x1b[1;36m                                           ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝\x1b[0m

\x1b[1;32mlogos@prts\x1b[0m:\x1b[1;34m~\x1b[0m$ `

onMounted(() => {
  if (terminalContainer.value) {
    terminal = new Terminal({
      theme: {
        background: '#1e1e1e',
        foreground: '#d4d4d4',
        cursor: '#aeafad',
        cursorAccent: '#1e1e1e',
        selectionBackground: '#264f78',
        black: '#1e1e1e',
        red: '#f44747',
        green: '#6a9955',
        yellow: '#dcdcaa',
        blue: '#569cd6',
        magenta: '#c586c0',
        cyan: '#4ec9b0',
        white: '#d4d4d4'
      },
      fontSize: 14,
      fontFamily: "'JetBrains Mono', 'Fira Code', monospace",
      cursorBlink: true,
      cursorStyle: 'bar',
      scrollback: 10000
    })

    fitAddon = new FitAddon()
    terminal.loadAddon(fitAddon)
    terminal.loadAddon(new WebLinksAddon())

    terminal.open(terminalContainer.value)
    fitAddon.fit()

    terminal.write(welcomeMessage)

    // 处理输入
    let currentLine = ''
    terminal.onData((data) => {
      if (data === '\r') {
        terminal?.write('\r\n')
        if (currentLine.trim()) {
          handleCommand(currentLine.trim())
        }
        terminal?.write('\x1b[1;32mlogos@prts\x1b[0m:\x1b[1;34m~\x1b[0m$ ')
        currentLine = ''
      } else if (data === '\x7f') {
        // Backspace
        if (currentLine.length > 0) {
          currentLine = currentLine.slice(0, -1)
          terminal?.write('\b \b')
        }
      } else {
        currentLine += data
        terminal?.write(data)
      }
    })

    // 监听窗口大小变化
    const resizeObserver = new ResizeObserver(() => {
      fitAddon?.fit()
    })
    resizeObserver.observe(terminalContainer.value)
  }
})

function handleCommand(cmd: string) {
  const args = cmd.split(' ')
  const command = args[0]

  switch (command) {
    case 'help':
      terminal?.writeln('\x1b[1;33mAvailable commands:\x1b[0m')
      terminal?.writeln('  help      - Show this help message')
      terminal?.writeln('  clear     - Clear the terminal')
      terminal?.writeln('  version   - Show Logos IDE version')
      terminal?.writeln('  status    - Show pipeline status')
      terminal?.writeln('  deploy    - Trigger deployment')
      break
    case 'clear':
      terminal?.clear()
      break
    case 'version':
      terminal?.writeln('\x1b[1;36mLogos IDE v0.1.0\x1b[0m')
      terminal?.writeln('PRTS DevOps Platform')
      break
    case 'status':
      terminal?.writeln('\x1b[1;32m● Production\x1b[0m  - Running (3 instances)')
      terminal?.writeln('\x1b[1;33m● Staging\x1b[0m     - Building...')
      terminal?.writeln('\x1b[1;34m● Development\x1b[0m - Idle')
      break
    case 'deploy':
      terminal?.writeln('\x1b[1;33mTriggering deployment...\x1b[0m')
      setTimeout(() => {
        terminal?.writeln('\x1b[1;32m✓ Deployment triggered successfully\x1b[0m')
      }, 500)
      break
    default:
      terminal?.writeln(`\x1b[1;31mCommand not found: ${command}\x1b[0m`)
      terminal?.writeln('Type "help" for available commands.')
  }
}

onUnmounted(() => {
  terminal?.dispose()
})
</script>

<template>
  <div class="terminal-view">
    <div class="terminal-header">
      <mdui-icon-terminal></mdui-icon-terminal>
      <span>终端</span>
      <div style="flex-grow: 1"></div>
      <mdui-button-icon>
        <mdui-icon-add></mdui-icon-add>
      </mdui-button-icon>
      <mdui-button-icon>
        <mdui-icon-close></mdui-icon-close>
      </mdui-button-icon>
    </div>
    <div ref="terminalContainer" class="terminal-container"></div>
  </div>
</template>

<style scoped>
.terminal-view {
  display: flex;
  flex-direction: column;
  height: 100%;
  background: #1e1e1e;
}

.terminal-header {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 12px;
  background: var(--mdui-color-surface-container);
  border-bottom: 1px solid var(--mdui-color-outline-variant);
  color: var(--mdui-color-on-surface);
}

.terminal-container {
  flex: 1;
  padding: 8px;
}
</style>
