class Curly::Multi

  def initialize
    @requests = []
  end

  def add(*requests)
    @requests += requests

    self
  end

  alias_method :enqueue, :add
end
