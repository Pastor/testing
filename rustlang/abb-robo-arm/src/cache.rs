use std::cell::RefCell;

use bloom::BloomFilter;

const FALSE_POSITIVE_RATE: f32 = 0.01;

pub(crate) struct Cache {
    filter: RefCell<BloomFilter>,
}

impl Default for Cache {
    fn default() -> Self {
        Cache::new_capacity(10000000)
    }
}

impl Cache {
    pub fn new_capacity(expected_num_items: u32) -> Self {
        Self::new(expected_num_items, FALSE_POSITIVE_RATE)
    }

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
