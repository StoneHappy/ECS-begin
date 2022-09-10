#include <cstdint>
#include <functional>
#include <gtest/gtest.h>
#include <entt/process/process.hpp>
#include <entt/process/scheduler.hpp>
#include <iostream>

struct Process0 : entt::process<Process0, int>
{
	Process0(std::function<void()> upd, std::function<void()> abort)
		: on_update(upd), on_aborted(abort) {}

	void update(delta_type, void*)
	{
		on_update();
	}
	 void aborted() {
        on_aborted();
    }
	std::function<void()> on_update;
	std::function<void()> on_aborted;
};

struct Process1 : entt::process<Process1, int>
{
	Process1(std::function<void()> upd)
		: on_update(upd) {}

	void update(delta_type, void*)
	{
		on_update();
		succeed();
	}
	std::function<void()> on_update;
	std::function<void()> on_aborted;
};

TEST(ECSbegin, process0)
{
	entt::scheduler<int> scheduler{};

	bool updated = false;
	bool aborted = false;

	scheduler.attach<Process0>( [&updated]() { updated = true; },
        [&aborted]() { aborted = true; });
	
	ASSERT_NE(scheduler.size(), 0u);
    ASSERT_FALSE(scheduler.empty());

	scheduler.update(0);
	scheduler.abort(true);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(aborted);
}

TEST(ECSbegin, process1)
{
	entt::scheduler<int> scheduler{};

	scheduler.attach<Process1>( []() { std::cout << 1 << std::endl; })
	.then<Process1>( []() { std::cout << 2 << std::endl; });

	while(!scheduler.empty()) {
        scheduler.update(0);
    }
}