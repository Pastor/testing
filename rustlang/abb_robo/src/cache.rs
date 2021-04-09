use bloom::BloomFilter;
use std::borrow::BorrowMut;

pub(crate) struct Cache<'a> {
    filter: &'a mut BloomFilter,
}

impl<'a> Default for Cache<'a> {
    fn default() -> Self {
        Cache::new(1000000, 0.01)
    }
}

impl<'a> Cache<'a> {
    fn new(expected_num_items: u32, false_positive_rate: f32) -> Self {
        let mut filter = BloomFilter::with_rate(false_positive_rate, expected_num_items);
        Cache {
            filter: filter.borrow_mut(),
        }
    }

    fn contains(&self, value: i32) -> bool {
        self.filter.contains(&value)
    }

    fn store(&mut self, value: i32) {
        self.filter.insert(&value)
    }
}
