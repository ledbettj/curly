class Curly::Multi

  def initialize
    @requests = []
  end

  def add(request)
    @requests.push(request)
  end

  alias_method :enqueue, :add
end
