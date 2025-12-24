import { contextBridge, ipcRenderer } from 'electron'

// 暴露给渲染进程的 API
contextBridge.exposeInMainWorld('electronAPI', {
  // 应用信息
  getVersion: () => ipcRenderer.invoke('app:version'),
  getPlatform: () => ipcRenderer.invoke('app:platform'),

  // Shell 操作
  openExternal: (url: string) => ipcRenderer.invoke('shell:openExternal', url),

  // 平台检测
  platform: process.platform,

  // 窗口控制 (预留)
  minimize: () => ipcRenderer.send('window:minimize'),
  maximize: () => ipcRenderer.send('window:maximize'),
  close: () => ipcRenderer.send('window:close'),
})

// 类型声明
declare global {
  interface Window {
    electronAPI: {
      getVersion: () => Promise<string>
      getPlatform: () => Promise<string>
      openExternal: (url: string) => Promise<void>
      platform: string
      minimize: () => void
      maximize: () => void
      close: () => void
    }
  }
}
