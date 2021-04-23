use std::cell::RefCell;

use bloom::BloomFilter;

pub(crate) struct Cache {
    filter: RefCell<BloomFilter>,
}

impl Default for Cache {
    fn default() -> Self {
        Cache::new(10000000, 0.01)
    }
}

impl Cache {
    fn new(expected_num_items: u32, false_positive_rate: f32) -> Self {
        Cache {
            filter: RefCell::new(BloomFilter::with_rate(
                false_positive_rate,
                expected_num_items,
            )),
        }
    }

    pub fn contains(&mut self, value: i32) -> bool {
        self.filter.get_mut().contains(&value)
    }

    pub fn store(&mut self, value: i32) {
        self.filter.get_mut().insert(&value)
    }
}
