#include <iostream>
#include <memory>
#include <vector>

class PersistentSegmentTree {
 private:
  struct Segment {
    int value = 0;

    std::shared_ptr<Segment> left;
    std::shared_ptr<Segment> right;

    Segment(int value, std::shared_ptr<Segment> left,
            std::shared_ptr<Segment> right);
  };

 public:
  PersistentSegmentTree(size_t size);
  PersistentSegmentTree(const std::vector<int>& arr);
  ~PersistentSegmentTree() = default;

  int SegmentValue(int version, int l, int r) const;
  int ChangeElement(int version, int i, int value);

 protected:
  void RecursiveBuild(std::shared_ptr<Segment>& node, int l, int r);
  void RecursiveBuild(std::shared_ptr<Segment>& node,
                      const std::vector<int>& arr, int l, int r);

  int SegmentValue(const std::shared_ptr<Segment>& node, int l, int r,
                   int seg_l, int seg_r) const;
  std::shared_ptr<Segment> ChangeElement(std::shared_ptr<Segment>& node, int i,
                                         int seg_l, int seg_r, int value);

 private:
  std::vector<std::shared_ptr<Segment>> versions;
  const size_t size;
};

PersistentSegmentTree::Segment::Segment(int value,
                                        std::shared_ptr<Segment> left,
                                        std::shared_ptr<Segment> right)
    : value{value}, left{left}, right{right} {}

PersistentSegmentTree::PersistentSegmentTree(size_t size) : size{size} {
  versions.push_back(std::make_shared<Segment>(0, std::shared_ptr<Segment>(),
      std::shared_ptr<Segment>()));

  RecursiveBuild(versions[0], 0, size);
}

PersistentSegmentTree::PersistentSegmentTree(const std::vector<int>& arr)
    : size{arr.size()} {
  versions.push_back(std::make_shared<Segment>(0, std::shared_ptr<Segment>(),
      std::shared_ptr<Segment>()));

  RecursiveBuild(versions[0], arr, 0, size);
}

void PersistentSegmentTree::RecursiveBuild(std::shared_ptr<Segment>& node,
                                           int l, int r) {
  int m = l + ((r - l) / 2);

  if (m - l > 0) {
    node->left = std::make_shared<Segment>(0, std::shared_ptr<Segment>(),
        std::shared_ptr<Segment>());
    RecursiveBuild(node->left, l, m);
  }
  if (r - m > 0) {
    node->right = std::make_shared<Segment>(0, std::shared_ptr<Segment>(),
        std::shared_ptr<Segment>());
    RecursiveBuild(node->right, m, r);
  }
}

void PersistentSegmentTree::RecursiveBuild(std::shared_ptr<Segment>& node,
                                           const std::vector<int>& arr, int l,
                                           int r) {
  if(r - l == 1) {
    node->value = arr[l];
    return;
  }

  int m = l + ((r - l) / 2);

  if (m - l > 0) {
    node->left = std::make_shared<Segment>(0, std::shared_ptr<Segment>(),
        std::shared_ptr<Segment>());
    RecursiveBuild(node->left, arr, l, m);
    node->value += node->left->value;
  }
  if (r - m > 0) {
    node->right = std::make_shared<Segment>(0, std::shared_ptr<Segment>(),
        std::shared_ptr<Segment>());
    RecursiveBuild(node->right, arr, m, r);
    node->value += node->right->value;
  }
}

int PersistentSegmentTree::SegmentValue(int version, int l, int r) const {
  return SegmentValue(versions[version], l, r, 0, size);
}

int PersistentSegmentTree::SegmentValue(const std::shared_ptr<Segment>& node,
                                        int l, int r, int seg_l,
                                        int seg_r) const {
  if (l <= seg_l && seg_r <= r) {
    return node->value;
  }
  if (seg_r <= l || r <= seg_l) {
    return 0;
  }
  int seg_m = seg_l + ((seg_r - seg_l) / 2);

  return SegmentValue(node->left, l, r, seg_l, seg_m) +
      SegmentValue(node->right, l, r, seg_m, seg_r);
}

int PersistentSegmentTree::ChangeElement(int version, int i, int value) {
  versions.push_back(ChangeElement(versions[version], i, 0, size, value));
  if (i < 0 || i >= size) {
    throw std::out_of_range("Invalid index");
  }
  return versions.size() - 1;
}

std::shared_ptr<PersistentSegmentTree::Segment>
PersistentSegmentTree::ChangeElement(std::shared_ptr<Segment>& node, int i,
                                     int seg_l, int seg_r, int value) {
  if (seg_l == i && i == seg_r - 1) {
    return std::make_shared<Segment>(value, std::shared_ptr<Segment>(),
        std::shared_ptr<Segment>());
  }
  int seg_m = seg_l + ((seg_r - seg_l) / 2);
  if (i < seg_m) {
    std::shared_ptr<Segment> new_left =
        ChangeElement(node->left, i, seg_l, seg_m, value);
    return std::make_shared<Segment>(new_left->value + node->right->value,
        new_left, node->right);
  }
  std::shared_ptr<Segment> new_right =
      ChangeElement(node->right, i, seg_m, seg_r, value);
  return std::make_shared<Segment>(node->left->value + new_right->value,
      node->left, new_right);
}

void TestSums(PersistentSegmentTree& tree, int version) {
  std::cout << tree.SegmentValue(version, 0, 1) << std::endl; // 1
  std::cout << tree.SegmentValue(version, 1, 2) << std::endl; // 2
  std::cout << tree.SegmentValue(version, 0, 2) << std::endl; // 3
  std::cout << tree.SegmentValue(version, 0, 3) << std::endl; // 6
  std::cout << tree.SegmentValue(version, 2, 4) << std::endl; // 7
  std::cout << tree.SegmentValue(version, 0, 5) << std::endl; // 15
}

void RunAllTests() {
  PersistentSegmentTree tree(std::vector<int>({1, 2, 3, 4, 5}));
  TestSums(tree, 0);
  tree.ChangeElement(0, 0, 2);
  TestSums(tree, 0);
  TestSums(tree, 1);
}

int main() {
  RunAllTests();

  return 0;
}
