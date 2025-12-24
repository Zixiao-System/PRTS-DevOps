import { createApp } from 'vue'
import { createPinia } from 'pinia'
import App from './App.vue'
import router from './router'

// MDUI 样式和组件
import 'mdui/mdui.css'
import 'mdui'

// MDUI 图标 - 按需导入
import '@mdui/icons/home.js'
import '@mdui/icons/search.js'
import '@mdui/icons/settings.js'
import '@mdui/icons/folder.js'
import '@mdui/icons/folder-open.js'
import '@mdui/icons/terminal.js'
import '@mdui/icons/code.js'
import '@mdui/icons/play-arrow.js'
import '@mdui/icons/stop.js'
import '@mdui/icons/refresh.js'
import '@mdui/icons/dashboard.js'
import '@mdui/icons/notifications.js'
import '@mdui/icons/account-circle.js'
import '@mdui/icons/menu.js'
import '@mdui/icons/close.js'
import '@mdui/icons/add.js'
import '@mdui/icons/remove.js'
import '@mdui/icons/edit.js'
import '@mdui/icons/delete.js'
import '@mdui/icons/save.js'
import '@mdui/icons/undo.js'
import '@mdui/icons/redo.js'
import '@mdui/icons/content-copy.js'
import '@mdui/icons/content-paste.js'
import '@mdui/icons/content-cut.js'
import '@mdui/icons/check.js'
import '@mdui/icons/error.js'
import '@mdui/icons/warning.js'
import '@mdui/icons/info.js'
import '@mdui/icons/cloud.js'
import '@mdui/icons/cloud-upload.js'
import '@mdui/icons/cloud-download.js'
import '@mdui/icons/sync.js'
import '@mdui/icons/build.js'
import '@mdui/icons/bug-report.js'
import '@mdui/icons/source.js'
import '@mdui/icons/commit.js'
import '@mdui/icons/merge.js'
import '@mdui/icons/call-split.js'

// 全局样式
import './styles/main.css'

const app = createApp(App)

app.use(createPinia())
app.use(router)

app.mount('#app')
