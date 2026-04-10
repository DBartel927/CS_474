# Reader/Writer Spinlock

## Why is reader_count atomic but writer_count is not?

reader_count has to be atomic because multiple reader threads can be in the critical section at the same time. That means they could all try to increment or decrement it at once, which would cause race conditions if it wasn’t atomic. writer_count doesn’t need to be atomic because only one writer is ever allowed in the critical section at a time. Since no two writers can run at once, there’s no risk of them modifying writer_count at the same time.

## How can writers be starved?

Writers can get starved in this solution because readers are allowed to keep entering the critical section as long as there is no writer currently holding the lock. The problem is that even if a writer is waiting, new readers are still allowed to come in. If readers keep showing up frequently enough, the number of readers might never drop to zero, which means the writer never gets a chance to acquire the lock. Therefore, a steady stream of readers can prevent a writer from ever running.