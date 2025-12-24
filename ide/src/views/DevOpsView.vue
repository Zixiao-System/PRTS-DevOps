<script setup lang="ts">
import { ref } from 'vue'

interface Pipeline {
  id: string
  name: string
  status: 'success' | 'running' | 'failed' | 'pending'
  branch: string
  commit: string
  duration: string
  triggeredBy: string
}

interface Metric {
  label: string
  value: string
  change: string
  trend: 'up' | 'down' | 'stable'
}

const pipelines = ref<Pipeline[]>([
  {
    id: '1',
    name: 'frontend-deploy',
    status: 'success',
    branch: 'main',
    commit: 'a1b2c3d',
    duration: '3m 42s',
    triggeredBy: 'Logos'
  },
  {
    id: '2',
    name: 'backend-build',
    status: 'running',
    branch: 'develop',
    commit: 'e4f5g6h',
    duration: '1m 20s',
    triggeredBy: 'Auto'
  },
  {
    id: '3',
    name: 'integration-test',
    status: 'failed',
    branch: 'feature/auth',
    commit: 'i7j8k9l',
    duration: '5m 10s',
    triggeredBy: 'PR #123'
  },
  {
    id: '4',
    name: 'staging-deploy',
    status: 'pending',
    branch: 'main',
    commit: 'a1b2c3d',
    duration: '-',
    triggeredBy: 'Manual'
  }
])

const metrics = ref<Metric[]>([
  { label: 'CPU 使用率', value: '42%', change: '+5%', trend: 'up' },
  { label: '内存使用', value: '6.2 GB', change: '-0.3 GB', trend: 'down' },
  { label: '构建成功率', value: '94%', change: '+2%', trend: 'up' },
  { label: '平均部署时间', value: '4m 32s', change: '-30s', trend: 'down' }
])

const getStatusColor = (status: string) => {
  switch (status) {
    case 'success': return 'var(--mdui-color-primary)'
    case 'running': return 'var(--mdui-color-tertiary)'
    case 'failed': return 'var(--mdui-color-error)'
    case 'pending': return 'var(--mdui-color-outline)'
    default: return 'var(--mdui-color-outline)'
  }
}

const getStatusText = (status: string) => {
  switch (status) {
    case 'success': return '成功'
    case 'running': return '运行中'
    case 'failed': return '失败'
    case 'pending': return '等待中'
    default: return status
  }
}
</script>

<template>
  <div class="devops-view">
    <!-- 顶部指标卡片 -->
    <div class="metrics-grid">
      <mdui-card
        v-for="metric in metrics"
        :key="metric.label"
        class="metric-card"
        variant="outlined"
      >
        <div class="metric-content">
          <span class="metric-label">{{ metric.label }}</span>
          <span class="metric-value">{{ metric.value }}</span>
          <span
            class="metric-change"
            :class="{ positive: metric.trend === 'down', negative: metric.trend === 'up' }"
          >
            {{ metric.change }}
          </span>
        </div>
      </mdui-card>
    </div>

    <!-- 流水线列表 -->
    <div class="pipelines-section">
      <div class="section-header">
        <h2>流水线</h2>
        <mdui-button variant="tonal">
          <mdui-icon-add slot="icon"></mdui-icon-add>
          新建流水线
        </mdui-button>
      </div>

      <mdui-list class="pipeline-list">
        <mdui-list-item
          v-for="pipeline in pipelines"
          :key="pipeline.id"
          class="pipeline-item"
        >
          <div class="pipeline-status" :style="{ background: getStatusColor(pipeline.status) }"></div>

          <div class="pipeline-info">
            <div class="pipeline-name">{{ pipeline.name }}</div>
            <div class="pipeline-meta">
              <span>
                <mdui-icon-source></mdui-icon-source>
                {{ pipeline.branch }}
              </span>
              <span>
                <mdui-icon-commit></mdui-icon-commit>
                {{ pipeline.commit }}
              </span>
              <span>{{ pipeline.triggeredBy }}</span>
            </div>
          </div>

          <div class="pipeline-status-text">
            {{ getStatusText(pipeline.status) }}
          </div>

          <div class="pipeline-duration">
            {{ pipeline.duration }}
          </div>

          <div class="pipeline-actions" slot="end-icon">
            <mdui-button-icon v-if="pipeline.status === 'pending'">
              <mdui-icon-play-arrow></mdui-icon-play-arrow>
            </mdui-button-icon>
            <mdui-button-icon v-else-if="pipeline.status === 'running'">
              <mdui-icon-stop></mdui-icon-stop>
            </mdui-button-icon>
            <mdui-button-icon v-else>
              <mdui-icon-refresh></mdui-icon-refresh>
            </mdui-button-icon>
          </div>
        </mdui-list-item>
      </mdui-list>
    </div>
  </div>
</template>

<style scoped>
.devops-view {
  padding: 24px;
  height: 100%;
  overflow-y: auto;
}

.metrics-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 16px;
  margin-bottom: 32px;
}

.metric-card {
  padding: 16px;
}

.metric-content {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.metric-label {
  font-size: 0.875rem;
  color: var(--mdui-color-on-surface-variant);
}

.metric-value {
  font-size: 1.75rem;
  font-weight: 500;
}

.metric-change {
  font-size: 0.875rem;
}

.metric-change.positive {
  color: var(--mdui-color-primary);
}

.metric-change.negative {
  color: var(--mdui-color-error);
}

.pipelines-section {
  background: var(--mdui-color-surface-container);
  border-radius: 12px;
  padding: 16px;
}

.section-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 16px;
}

.section-header h2 {
  margin: 0;
  font-size: 1.25rem;
  font-weight: 500;
}

.pipeline-list {
  background: transparent;
}

.pipeline-item {
  display: flex;
  align-items: center;
  gap: 16px;
  padding: 12px 16px;
  border-radius: 8px;
  margin-bottom: 8px;
  background: var(--mdui-color-surface);
}

.pipeline-status {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  flex-shrink: 0;
}

.pipeline-info {
  flex: 1;
  min-width: 0;
}

.pipeline-name {
  font-weight: 500;
  margin-bottom: 4px;
}

.pipeline-meta {
  display: flex;
  gap: 16px;
  font-size: 0.875rem;
  color: var(--mdui-color-on-surface-variant);
}

.pipeline-meta span {
  display: flex;
  align-items: center;
  gap: 4px;
}

.pipeline-meta mdui-icon-source,
.pipeline-meta mdui-icon-commit {
  font-size: 14px;
}

.pipeline-status-text {
  width: 80px;
  text-align: center;
  font-size: 0.875rem;
}

.pipeline-duration {
  width: 80px;
  text-align: right;
  font-size: 0.875rem;
  color: var(--mdui-color-on-surface-variant);
}

.pipeline-actions {
  display: flex;
  gap: 4px;
}
</style>
