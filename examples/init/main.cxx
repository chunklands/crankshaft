#include <crankshaft.h>
#include <iostream>

int main() {
  // cra_init([](int status, void* data) {
  //   assert(status == CRA_OK);
  // }, nullptr);

  // cra_engine_t engine;
  // cra_engine_new(&engine);

  // cra_engine_init(engine, [](cra_engine_t engine) {
  //   std::cout << "init done" << std::endl;
  // });

  // cra_window_new_params_s params = {
  //   .width = 1024,
  //   .height = 768,
  //   .title = "CRANKSHAFT"
  // };

  // cra_window_new(engine, &params, [](cra_window_t window) {
  //   std::cout << "init window done" << std::endl;

  //   int should_close = 0;
  //   do {
  //     cra_poll_events();
  //     cra_window_shouldclose(window, &should_close);
  //   } while (!should_close);

  //   cra_engine_t engine;
  //   cra_window_get_engine(window, &engine);
  //   cra_engine_stop(engine);
  // });

  // cra_engine_loop_wait(engine);
  // cra_engine_delete(engine);
  // cra_destroy();
}