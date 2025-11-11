use std::time::Instant;

fn main() {
    const SEED: i64 = 0;

    let _ = opensimplex2::fast::noise2(SEED, 0.0, 0.0);

    for _ in 0..10 {
        let iteration_time = Instant::now();

        for x in 0..8000 {
            for y in 0..8000 {
                let _ = opensimplex2::fast::noise2(SEED, x as f64, y as f64);
            }
        }

        println!(
            "Rust Impl 2D: {} msec",
            iteration_time.elapsed().as_millis()
        );
    }
}
