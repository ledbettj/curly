class Curly::Request
  class << self

    def get(url, opts = {})
      opts[:method] = :get
      Curly::Request.new(url, opts).run
    end

    def post(url, opts = {})
      opts[:method] = :post
      Curly::Request.new(url, opts).run
    end

    def put(url, opts = {})
      opts[:method] = :put
      Curly::Request.new(url, opts).run
    end

    def delete(url, opts = {})
      opts[:method] = :delete
      Curly::Request.new(url, opts).run
    end

  end

  attr_accessor :url
  attr_accessor :body, :headers, :method, :params, :timeout

  def initialize(url, opts = {})
    @url     = url
    @body    = opts[:body]
    @headers = opts[:headers]
    @method  = opts[:method]
    @params  = opts[:params]
    @timeout = opts[:timeout]
  end
end
