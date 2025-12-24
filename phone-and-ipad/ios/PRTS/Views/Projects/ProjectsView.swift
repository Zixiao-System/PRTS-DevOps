import SwiftUI

struct ProjectsView: View {
    @StateObject private var viewModel = ProjectsViewModel()
    @State private var showAddProject = false
    @State private var searchText = ""

    var filteredProjects: [Project] {
        if searchText.isEmpty {
            return viewModel.projects
        }
        return viewModel.projects.filter {
            $0.name.localizedCaseInsensitiveContains(searchText)
        }
    }

    var body: some View {
        NavigationStack {
            List {
                ForEach(filteredProjects) { project in
                    NavigationLink {
                        ProjectDetailView(project: project)
                    } label: {
                        ProjectRow(project: project)
                    }
                }
            }
            .listStyle(.plain)
            .searchable(text: $searchText, prompt: "搜索项目")
            .navigationTitle("项目")
            .toolbar {
                ToolbarItem(placement: .topBarTrailing) {
                    Button {
                        showAddProject = true
                    } label: {
                        Image(systemName: "plus")
                    }
                }
            }
            .refreshable {
                await viewModel.refresh()
            }
            .sheet(isPresented: $showAddProject) {
                AddProjectView { project in
                    viewModel.addProject(project)
                }
            }
        }
    }
}

// MARK: - Project Row
struct ProjectRow: View {
    let project: Project

    var body: some View {
        HStack(spacing: 12) {
            // 项目图标
            Text(project.emoji)
                .font(.title)
                .frame(width: 50, height: 50)
                .background(Color(.systemGray5))
                .cornerRadius(10)

            VStack(alignment: .leading, spacing: 4) {
                Text(project.name)
                    .font(.headline)

                Text(project.description)
                    .font(.subheadline)
                    .foregroundStyle(.secondary)
                    .lineLimit(1)

                HStack(spacing: 12) {
                    Label("\(project.pipelinesCount)", systemImage: "arrow.triangle.branch")
                    Label("\(project.membersCount)", systemImage: "person.2")
                }
                .font(.caption)
                .foregroundStyle(.secondary)
            }
        }
        .padding(.vertical, 4)
    }
}

// MARK: - Project Detail
struct ProjectDetailView: View {
    let project: Project

    var body: some View {
        ScrollView {
            VStack(spacing: 20) {
                // 项目头部
                VStack(spacing: 12) {
                    Text(project.emoji)
                        .font(.system(size: 60))

                    Text(project.name)
                        .font(.title)
                        .fontWeight(.bold)

                    Text(project.description)
                        .foregroundStyle(.secondary)
                        .multilineTextAlignment(.center)
                }
                .padding()

                // 统计信息
                HStack(spacing: 0) {
                    StatItem(value: "\(project.pipelinesCount)", label: "流水线")
                    Divider().frame(height: 40)
                    StatItem(value: "\(project.membersCount)", label: "成员")
                    Divider().frame(height: 40)
                    StatItem(value: project.lastActivity, label: "最后活动")
                }
                .padding()
                .background(Color(.systemGray6))
                .cornerRadius(12)
                .padding(.horizontal)

                // 快捷操作
                VStack(spacing: 12) {
                    NavigationLink {
                        PipelineListView()
                    } label: {
                        HStack {
                            Image(systemName: "arrow.triangle.branch")
                            Text("查看流水线")
                            Spacer()
                            Image(systemName: "chevron.right")
                        }
                        .padding()
                        .background(Color(.systemGray6))
                        .cornerRadius(12)
                    }
                    .buttonStyle(.plain)

                    Button {
                        // 项目设置
                    } label: {
                        HStack {
                            Image(systemName: "gearshape")
                            Text("项目设置")
                            Spacer()
                            Image(systemName: "chevron.right")
                        }
                        .padding()
                        .background(Color(.systemGray6))
                        .cornerRadius(12)
                    }
                    .buttonStyle(.plain)
                }
                .padding(.horizontal)
            }
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct StatItem: View {
    let value: String
    let label: String

    var body: some View {
        VStack(spacing: 4) {
            Text(value)
                .font(.title2)
                .fontWeight(.bold)
            Text(label)
                .font(.caption)
                .foregroundStyle(.secondary)
        }
        .frame(maxWidth: .infinity)
    }
}

// MARK: - Add Project
struct AddProjectView: View {
    @Environment(\.dismiss) private var dismiss
    @State private var name = ""
    @State private var description = ""
    @State private var selectedEmoji = "📦"
    let onAdd: (Project) -> Void

    let emojis = ["📦", "🚀", "⚡️", "🔧", "🎯", "💡", "🔥", "✨"]

    var body: some View {
        NavigationStack {
            Form {
                Section("基本信息") {
                    TextField("项目名称", text: $name)

                    TextField("描述", text: $description, axis: .vertical)
                        .lineLimit(3...6)
                }

                Section("图标") {
                    LazyVGrid(columns: Array(repeating: GridItem(.flexible()), count: 4), spacing: 16) {
                        ForEach(emojis, id: \.self) { emoji in
                            Button {
                                selectedEmoji = emoji
                            } label: {
                                Text(emoji)
                                    .font(.title)
                                    .frame(width: 50, height: 50)
                                    .background(selectedEmoji == emoji ? Color.blue.opacity(0.3) : Color(.systemGray5))
                                    .cornerRadius(10)
                            }
                            .buttonStyle(.plain)
                        }
                    }
                    .padding(.vertical, 8)
                }
            }
            .navigationTitle("新建项目")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .topBarLeading) {
                    Button("取消") {
                        dismiss()
                    }
                }
                ToolbarItem(placement: .topBarTrailing) {
                    Button("创建") {
                        let project = Project(
                            id: UUID().uuidString,
                            name: name,
                            description: description,
                            emoji: selectedEmoji,
                            pipelinesCount: 0,
                            membersCount: 1,
                            lastActivity: "刚刚"
                        )
                        onAdd(project)
                        dismiss()
                    }
                    .disabled(name.isEmpty)
                }
            }
        }
    }
}

// MARK: - ViewModel
class ProjectsViewModel: ObservableObject {
    @Published var projects: [Project] = []

    init() {
        loadProjects()
    }

    func refresh() async {
        try? await Task.sleep(nanoseconds: 1_000_000_000)
        await MainActor.run {
            loadProjects()
        }
    }

    func addProject(_ project: Project) {
        projects.insert(project, at: 0)
    }

    private func loadProjects() {
        projects = [
            Project(id: "1", name: "PRTS Frontend", description: "前端 Web 应用", emoji: "🌐", pipelinesCount: 5, membersCount: 8, lastActivity: "2小时前"),
            Project(id: "2", name: "PRTS Backend", description: "后端 API 服务", emoji: "⚡️", pipelinesCount: 3, membersCount: 6, lastActivity: "30分钟前"),
            Project(id: "3", name: "Mobile App", description: "iOS & Android 移动应用", emoji: "📱", pipelinesCount: 4, membersCount: 4, lastActivity: "1天前"),
            Project(id: "4", name: "Infrastructure", description: "基础设施配置", emoji: "🔧", pipelinesCount: 2, membersCount: 3, lastActivity: "3天前")
        ]
    }
}

#Preview {
    ProjectsView()
        .preferredColorScheme(.dark)
}
