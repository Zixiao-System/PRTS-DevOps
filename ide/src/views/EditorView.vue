<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import * as monaco from 'monaco-editor'

const editorContainer = ref<HTMLElement | null>(null)
let editor: monaco.editor.IStandaloneCodeEditor | null = null

// 示例代码
const sampleCode = `// Welcome to Logos IDE
// PRTS DevOps Platform - 智慧与洞察的化身

import { createApp } from 'vue'
import { defineStore } from 'pinia'

interface Pipeline {
  id: string
  name: string
  status: 'pending' | 'running' | 'success' | 'failed'
  stages: Stage[]
}

interface Stage {
  name: string
  jobs: Job[]
}

interface Job {
  name: string
  script: string[]
}

export const usePipelineStore = defineStore('pipeline', {
  state: () => ({
    pipelines: [] as Pipeline[],
    currentPipeline: null as Pipeline | null
  }),

  actions: {
    async fetchPipelines() {
      // Fetch pipelines from API
      const response = await fetch('/api/v1/pipelines')
      this.pipelines = await response.json()
    },

    async triggerPipeline(id: string) {
      await fetch(\`/api/v1/pipelines/\${id}/trigger\`, {
        method: 'POST'
      })
    }
  }
})
`

onMounted(() => {
  if (editorContainer.value) {
    // 设置 Monaco 主题
    monaco.editor.defineTheme('logos-dark', {
      base: 'vs-dark',
      inherit: true,
      rules: [],
      colors: {
        'editor.background': '#1e1e1e',
        'editor.foreground': '#d4d4d4',
        'editorLineNumber.foreground': '#858585',
        'editorCursor.foreground': '#aeafad',
        'editor.selectionBackground': '#264f78',
        'editor.lineHighlightBackground': '#2a2d2e'
      }
    })

    editor = monaco.editor.create(editorContainer.value, {
      value: sampleCode,
      language: 'typescript',
      theme: 'logos-dark',
      fontSize: 14,
      fontFamily: "'JetBrains Mono', 'Fira Code', monospace",
      fontLigatures: true,
      minimap: { enabled: true },
      scrollBeyondLastLine: false,
      automaticLayout: true,
      tabSize: 2,
      wordWrap: 'on',
      lineNumbers: 'on',
      renderWhitespace: 'selection',
      cursorBlinking: 'smooth',
      smoothScrolling: true
    })
  }
})

onUnmounted(() => {
  editor?.dispose()
})
</script>

<template>
  <div class="editor-view">
    <!-- 文件标签栏 -->
    <div class="tab-bar">
      <mdui-tabs value="main.ts">
        <mdui-tab value="main.ts">
          <mdui-icon-code slot="icon"></mdui-icon-code>
          main.ts
        </mdui-tab>
        <mdui-tab value="App.vue">
          <mdui-icon-code slot="icon"></mdui-icon-code>
          App.vue
        </mdui-tab>
      </mdui-tabs>
    </div>

    <!-- 编辑器容器 -->
    <div ref="editorContainer" class="editor-container"></div>
  </div>
</template>

<style scoped>
.editor-view {
  display: flex;
  flex-direction: column;
  height: 100%;
}

.tab-bar {
  background: var(--mdui-color-surface-container);
  border-bottom: 1px solid var(--mdui-color-outline-variant);
}

.editor-container {
  flex: 1;
  overflow: hidden;
}
</style>
