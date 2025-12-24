import SwiftUI

struct PipelineListView: View {
    @StateObject private var viewModel = PipelineViewModel()

    var body: some View {
        NavigationStack {
            List {
                ForEach(viewModel.pipelines) { pipeline in
                    NavigationLink {
                        PipelineDetailView(pipeline: pipeline)
                    } label: {
                        PipelineRow(pipeline: pipeline)
                    }
                }
            }
            .listStyle(.plain)
            .navigationTitle("流水线")
            .toolbar {
                ToolbarItem(placement: .topBarTrailing) {
                    Button {
                        // 新建流水线
                    } label: {
                        Image(systemName: "plus")
                    }
                }
            }
            .refreshable {
                await viewModel.refresh()
            }
        }
    }
}

// MARK: - Pipeline Row
struct PipelineRow: View {
    let pipeline: Pipeline

    var body: some View {
        HStack(spacing: 12) {
            // 状态指示器
            Circle()
                .fill(pipeline.status.color)
                .frame(width: 10, height: 10)

            VStack(alignment: .leading, spacing: 4) {
                Text(pipeline.name)
                    .font(.headline)

                HStack(spacing: 8) {
                    Label(pipeline.branch, systemImage: "arrow.triangle.branch")
                    Text("·")
                    Text(pipeline.duration)
                }
                .font(.caption)
                .foregroundStyle(.secondary)
            }

            Spacer()

            Text(pipeline.status.rawValue)
                .font(.caption)
                .padding(.horizontal, 8)
                .padding(.vertical, 4)
                .background(pipeline.status.color.opacity(0.2))
                .foregroundStyle(pipeline.status.color)
                .cornerRadius(4)
        }
        .padding(.vertical, 4)
    }
}

// MARK: - Pipeline Detail
struct PipelineDetailView: View {
    let pipeline: Pipeline
    @State private var showTriggerConfirm = false

    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 20) {
                // 状态卡片
                HStack {
                    VStack(alignment: .leading) {
                        Text("状态")
                            .font(.caption)
                            .foregroundStyle(.secondary)
                        Text(pipeline.status.rawValue)
                            .font(.title2)
                            .fontWeight(.bold)
                            .foregroundStyle(pipeline.status.color)
                    }

                    Spacer()

                    VStack(alignment: .trailing) {
                        Text("耗时")
                            .font(.caption)
                            .foregroundStyle(.secondary)
                        Text(pipeline.duration)
                            .font(.title2)
                            .fontWeight(.bold)
                    }
                }
                .padding()
                .background(Color(.systemGray6))
                .cornerRadius(12)

                // 分支信息
                VStack(alignment: .leading, spacing: 8) {
                    Text("分支")
                        .font(.headline)

                    HStack {
                        Image(systemName: "arrow.triangle.branch")
                        Text(pipeline.branch)
                    }
                    .padding()
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .background(Color(.systemGray6))
                    .cornerRadius(8)
                }

                // 阶段列表
                VStack(alignment: .leading, spacing: 12) {
                    Text("阶段")
                        .font(.headline)

                    ForEach(["Build", "Test", "Deploy"], id: \.self) { stage in
                        HStack {
                            Image(systemName: "checkmark.circle.fill")
                                .foregroundStyle(.green)
                            Text(stage)
                            Spacer()
                            Text("1m 20s")
                                .foregroundStyle(.secondary)
                        }
                        .padding()
                        .background(Color(.systemGray6))
                        .cornerRadius(8)
                    }
                }

                // 操作按钮
                HStack(spacing: 16) {
                    Button {
                        showTriggerConfirm = true
                    } label: {
                        Label("重新运行", systemImage: "arrow.clockwise")
                            .frame(maxWidth: .infinity)
                    }
                    .buttonStyle(.borderedProminent)

                    Button {
                        // 查看日志
                    } label: {
                        Label("查看日志", systemImage: "doc.text")
                            .frame(maxWidth: .infinity)
                    }
                    .buttonStyle(.bordered)
                }
            }
            .padding()
        }
        .navigationTitle(pipeline.name)
        .navigationBarTitleDisplayMode(.inline)
        .confirmationDialog("确认重新运行？", isPresented: $showTriggerConfirm) {
            Button("重新运行") {
                // 触发流水线
            }
            Button("取消", role: .cancel) {}
        }
    }
}

// MARK: - ViewModel
class PipelineViewModel: ObservableObject {
    @Published var pipelines: [Pipeline] = []

    init() {
        loadPipelines()
    }

    func refresh() async {
        try? await Task.sleep(nanoseconds: 1_000_000_000)
        await MainActor.run {
            loadPipelines()
        }
    }

    private func loadPipelines() {
        pipelines = [
            Pipeline(id: "1", name: "frontend-deploy", status: .success, branch: "main", duration: "3m 42s"),
            Pipeline(id: "2", name: "backend-build", status: .running, branch: "develop", duration: "1m 20s"),
            Pipeline(id: "3", name: "integration-test", status: .failed, branch: "feature/auth", duration: "5m 10s"),
            Pipeline(id: "4", name: "staging-deploy", status: .pending, branch: "main", duration: "-"),
            Pipeline(id: "5", name: "e2e-test", status: .success, branch: "main", duration: "12m 5s")
        ]
    }
}

#Preview {
    PipelineListView()
        .preferredColorScheme(.dark)
}
