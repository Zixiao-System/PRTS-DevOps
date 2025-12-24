import Foundation

class APIClient {
    static let shared = APIClient()

    private let baseURL: URL
    private let session: URLSession
    private var authToken: String?

    private init() {
        // TODO: 从配置读取
        self.baseURL = URL(string: "http://localhost:8000/api/v1")!
        self.session = URLSession.shared
    }

    // MARK: - Auth
    func setAuthToken(_ token: String) {
        self.authToken = token
    }

    func clearAuthToken() {
        self.authToken = nil
    }

    // MARK: - Generic Request
    func request<T: Codable>(
        endpoint: String,
        method: HTTPMethod = .get,
        body: Encodable? = nil
    ) async throws -> T {
        let url = baseURL.appendingPathComponent(endpoint)
        var request = URLRequest(url: url)
        request.httpMethod = method.rawValue
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")

        if let token = authToken {
            request.setValue("Bearer \(token)", forHTTPHeaderField: "Authorization")
        }

        if let body = body {
            request.httpBody = try JSONEncoder().encode(body)
        }

        let (data, response) = try await session.data(for: request)

        guard let httpResponse = response as? HTTPURLResponse else {
            throw APIClientError.invalidResponse
        }

        guard (200...299).contains(httpResponse.statusCode) else {
            throw APIClientError.httpError(statusCode: httpResponse.statusCode)
        }

        let decoder = JSONDecoder()
        decoder.dateDecodingStrategy = .iso8601
        return try decoder.decode(T.self, from: data)
    }

    // MARK: - Pipelines
    func getPipelines() async throws -> [Pipeline] {
        try await request(endpoint: "pipelines")
    }

    func getPipeline(id: String) async throws -> Pipeline {
        try await request(endpoint: "pipelines/\(id)")
    }

    func triggerPipeline(id: String) async throws -> Pipeline {
        try await request(endpoint: "pipelines/\(id)/trigger", method: .post)
    }

    func cancelPipeline(id: String) async throws -> Pipeline {
        try await request(endpoint: "pipelines/\(id)/cancel", method: .post)
    }

    // MARK: - Alerts
    func getAlerts() async throws -> [AlertItem] {
        try await request(endpoint: "alerts")
    }

    func acknowledgeAlert(id: String) async throws -> AlertItem {
        try await request(endpoint: "alerts/\(id)/acknowledge", method: .post)
    }

    // MARK: - Projects
    func getProjects() async throws -> [Project] {
        try await request(endpoint: "projects")
    }

    func createProject(_ project: Project) async throws -> Project {
        try await request(endpoint: "projects", method: .post, body: project)
    }

    // MARK: - Metrics
    func getMetrics() async throws -> Metrics {
        try await request(endpoint: "monitoring/metrics")
    }

    // MARK: - Push Token
    func registerPushToken(_ token: String) async throws {
        struct PushTokenRequest: Codable {
            let token: String
            let platform: String = "ios"
        }
        let _: EmptyResponse = try await request(
            endpoint: "push/register",
            method: .post,
            body: PushTokenRequest(token: token)
        )
    }
}

// MARK: - HTTP Method
enum HTTPMethod: String {
    case get = "GET"
    case post = "POST"
    case put = "PUT"
    case patch = "PATCH"
    case delete = "DELETE"
}

// MARK: - Errors
enum APIClientError: Error {
    case invalidResponse
    case httpError(statusCode: Int)
    case decodingError(Error)
}

// MARK: - Empty Response
struct EmptyResponse: Codable {}
