<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'

const router = useRouter()
const currentRoute = ref('/')
const drawerOpen = ref(false)

// 导航项
const navItems = [
  { path: '/', icon: 'mdui-icon-code', label: '编辑器' },
  { path: '/terminal', icon: 'mdui-icon-terminal', label: '终端' },
  { path: '/devops', icon: 'mdui-icon-dashboard', label: 'DevOps' },
  { path: '/settings', icon: 'mdui-icon-settings', label: '设置' }
]

const navigateTo = (path: string) => {
  currentRoute.value = path
  router.push(path)
}

onMounted(async () => {
  // 获取平台信息
  if (window.electronAPI) {
    const version = await window.electronAPI.getVersion()
    console.log('Logos IDE version:', version)
  }
})
</script>

<template>
  <mdui-layout class="app-layout">
    <!-- 侧边栏 -->
    <mdui-navigation-drawer
      :open="drawerOpen"
      @close="drawerOpen = false"
      class="app-drawer"
    >
      <div class="drawer-header">
        <h2>Logos IDE</h2>
        <span class="version">v0.1.0</span>
      </div>

      <mdui-list>
        <mdui-list-item
          v-for="item in navItems"
          :key="item.path"
          :active="currentRoute === item.path"
          @click="navigateTo(item.path)"
        >
          <component :is="item.icon" slot="icon"></component>
          {{ item.label }}
        </mdui-list-item>
      </mdui-list>

      <mdui-divider></mdui-divider>

      <div class="drawer-footer">
        <mdui-list>
          <mdui-list-item>
            <mdui-icon-notifications slot="icon"></mdui-icon-notifications>
            通知
            <mdui-badge slot="end-icon">3</mdui-badge>
          </mdui-list-item>
          <mdui-list-item>
            <mdui-icon-account-circle slot="icon"></mdui-icon-account-circle>
            账户
          </mdui-list-item>
        </mdui-list>
      </div>
    </mdui-navigation-drawer>

    <!-- 顶部应用栏 -->
    <mdui-top-app-bar class="app-bar">
      <mdui-button-icon @click="drawerOpen = !drawerOpen">
        <mdui-icon-menu></mdui-icon-menu>
      </mdui-button-icon>

      <mdui-top-app-bar-title>Logos IDE</mdui-top-app-bar-title>

      <div style="flex-grow: 1"></div>

      <!-- 搜索 -->
      <mdui-text-field
        class="search-field"
        placeholder="搜索文件、命令..."
        variant="outlined"
      >
        <mdui-icon-search slot="icon"></mdui-icon-search>
      </mdui-text-field>

      <!-- 操作按钮 -->
      <mdui-button-icon>
        <mdui-icon-play-arrow></mdui-icon-play-arrow>
      </mdui-button-icon>

      <mdui-button-icon>
        <mdui-icon-bug-report></mdui-icon-bug-report>
      </mdui-button-icon>

      <mdui-button-icon>
        <mdui-icon-source></mdui-icon-source>
      </mdui-button-icon>
    </mdui-top-app-bar>

    <!-- 主内容区 -->
    <mdui-layout-main class="app-main">
      <router-view v-slot="{ Component }">
        <transition name="fade" mode="out-in">
          <component :is="Component" />
        </transition>
      </router-view>
    </mdui-layout-main>

    <!-- 底部状态栏 -->
    <div class="status-bar">
      <div class="status-left">
        <span class="status-item">
          <mdui-icon-source></mdui-icon-source>
          main
        </span>
        <span class="status-item">
          <mdui-icon-sync></mdui-icon-sync>
        </span>
      </div>
      <div class="status-right">
        <span class="status-item">UTF-8</span>
        <span class="status-item">TypeScript</span>
        <span class="status-item">Ln 1, Col 1</span>
      </div>
    </div>
  </mdui-layout>
</template>

<style scoped>
.app-layout {
  height: 100vh;
  display: flex;
  flex-direction: column;
}

.app-drawer {
  --mdui-color-surface: var(--mdui-color-surface-container);
}

.drawer-header {
  padding: 24px 16px;
  border-bottom: 1px solid var(--mdui-color-outline-variant);
}

.drawer-header h2 {
  margin: 0;
  font-size: 1.5rem;
  font-weight: 500;
}

.drawer-header .version {
  color: var(--mdui-color-on-surface-variant);
  font-size: 0.875rem;
}

.drawer-footer {
  margin-top: auto;
}

.app-bar {
  --mdui-color-surface: var(--mdui-color-surface-container);
}

.search-field {
  max-width: 400px;
  margin: 0 16px;
}

.app-main {
  flex: 1;
  overflow: hidden;
  background: var(--mdui-color-surface);
}

.status-bar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  height: 24px;
  padding: 0 8px;
  font-size: 12px;
  background: var(--mdui-color-primary);
  color: var(--mdui-color-on-primary);
}

.status-left,
.status-right {
  display: flex;
  align-items: center;
  gap: 12px;
}

.status-item {
  display: flex;
  align-items: center;
  gap: 4px;
}

.status-item mdui-icon-source,
.status-item mdui-icon-sync {
  font-size: 14px;
}

/* 路由过渡动画 */
.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.15s ease;
}

.fade-enter-from,
.fade-leave-to {
  opacity: 0;
}
</style>
